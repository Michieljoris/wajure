const util = require('util');
const fs = require('fs');

var str = "";

var memory;
var runtime;

var page_size = 64 * 1024;
var max_page_count = 100;
var max_size = page_size * max_page_count;
var initial_page_count = 2
var page_count = initial_page_count;

// var i8, i16, i32;


function makeLogString(memory, offset) {
    return function(offset) {
        var bytes = new Uint8Array(memory.buffer, offset);
        var length = 0;
        while (bytes[length] != 0) length++;
        var bytes = new Uint8Array(memory.buffer, offset, length);
        var string = new TextDecoder('utf8').decode(bytes);
        console.log(string);
    }
}

const runtime_error_codes = {
    TOO_FEW_ARGS: 0,
    TOO_MANY_ARGS: 1,
    NOT_A_FN: 2
}

function make_runtime_error_fn(memory, offset) {
    return function(err_no, offset) {
        var bytes = new Uint8Array(memory.buffer, offset);
        var length = 0;
        while (bytes[length] != 0) length++;
        var bytes = new Uint8Array(memory.buffer, offset, length);
        var string = new TextDecoder('utf8').decode(bytes);
        switch (err_no) {
            case runtime_error_codes.TOO_FEW_ARGS:
                string = "Too few args passed to " + string; break;
            case runtime_error_codes.TOO_MANY_ARGS:
                string = "Too many args passed to " + string; break;
            case runtime_error_codes.NOT_A_FN:
                string = "Not a fn!!!" + string; break;
            default: string = "Unknown runtime error code: " + err_no + " " + string;
        }
        throw string;
    }
}

function makeLogStringN(memory, offset, length) {
    return function(offset, length) {
        var bytes = new Uint8Array(memory.buffer, offset, length);
        var string = new TextDecoder('utf8').decode(bytes);
        console.log(string);
    }
}

async function run_wajure_fn(env, module, fn_name, ...args) {
    let stack_pointer = env.stack_pointer_global.value;
    // console.log("stack_pointer: ", stack_pointer);

    let args_count = args.length;
    let wajure_args = [];
    for (i = 0;  i < args_count; i++) {
        let arg = env.runtime.exports.make_lval_num(args[i]);
        wajure_args[i] = arg;
    }

    let closure_pointer = 0;
    try {
        module.instance.exports[fn_name](closure_pointer, ... wajure_args);
    } catch (e) {
        console.log("RUNTIME ERROR");
        console.log(e);
    }

}


function get_custom_section(module, section) {
    var nameSections = WebAssembly.Module.customSections(module, section);

    if (nameSections.length != 0) {
        // console.log("Module contains a custom section: ", section);
        var string = new TextDecoder('utf8').decode(nameSections[0]);
        // console.log(string);
        return string;
    };

}

function parse_symbol_info(a) {
    let result = { name: a[0],
                   type: a[1],
                   ptr : a[2] }
    if (result.type == "Function") {
        result.fn_table_index = a[3]
        result.param_count = a[4];
        result.rest_arg_index = a[5];
    }
    return result;
}

var fn_offset = { offset_type: "fn_table_index",  offset: "fn_table_offset", start: "fn_table_start" }
var data_offset = { offset_type: "ptr",  offset: "data_offset", start: "data_start" };

function parse_deps(deps) {
    return deps.split("\n")
               .filter(l => l.length > 0)
               .map(l => l.split("/"))
               .map(a => {
                   let [prefix, namespace] = a[0].split(":");
                   let global = a[0] + "/" + a[1];
                   return Object.assign({},
                       prefix == "fn" ? fn_offset : data_offset,
                       { namespace, name: a[1],  global});
               })
               .reduce((acc, v) => {
                   acc[v.namespace] = acc[v.namespace] || []
                   acc[v.namespace].push(v);
                   return acc;
               }, {})
}

function parse_custom_sections(module) {
    let symbol_table = get_custom_section(module, "symbol_table");
    if (symbol_table == undefined) return { error: "Missing symbol table"};

    symbol_table = symbol_table
        .split("\n")
        .filter(l => l.length > 0)
        .map(symbol_str => symbol_str.split(","))
        .map(parse_symbol_info)
        .reduce((acc, s) => {
            acc[s.name] = s;
            return acc;
        }, {})

    let deps = get_custom_section(module, "deps");
    if (deps == undefined) return { error: "Missing deps"};
    // console.log("DEPS:", deps);
    deps = parse_deps(deps);

    let data_size = get_custom_section(module, "data_size");
    if (data_size == undefined) return { error: "Missing data_size"};

    let fn_table_size = get_custom_section(module, "fn_table_size");
    if (fn_table_size == undefined) return { error: "Missing fn_table_size"};

    return { symbol_table, deps, data_size, fn_table_size };
}

async function instantiate_runtime(env) {
    let { config } = env;
    let buf = fs.readFileSync(config.runtime_wasm);
    let  runtime;
    let importObject = {
        env: {
            _putchar: arg => {
                process.stdout.write(String.fromCharCode(arg));
            } ,
            grow_memory: () => {
                if (++page_count > max_page_count) {
                    console.log("Error: can't allocate memory beyond max (nodejs) of " +
                                max_page_count * page_size / 1024 + "kb\n");
                    // throw "Error: can't allocate memory beyond max\n";
                    return 0;
                }
                // console.log("Grow_memory from ", (page_count * page_size)/1024, "kb to ", ((page_count+1) * page_size)/1024 , "kb");
                memory = runtime.exports.memory.grow(1);
                return 1;
            },
            get_mem_end: () => page_count * page_size
        }
    };
    runtime = await WebAssembly.instantiate(new Uint8Array(buf), importObject).
        then(res => res.instance);

    runtime.exports.init_malloc();
    let stack_pointer = runtime.exports._malloc(config.stack_size); //8MB stack

    runtime.exports.init_lispy_mempools(3200, 3200, 3200);
    env.stack_pointer = stack_pointer;
    env.stack_pointer_global = new WebAssembly.Global({ value: 'i32', mutable: true }, stack_pointer);
    env.runtime = runtime;
    env.data_start = env.runtime.exports.__data_end.value;
    env.fn_table_start = 0;

    env.runtime_error_fn = make_runtime_error_fn(env.runtime.exports.memory);
    //TODO: we can't set maximum as optional with BinaryenSetFunctionTable as we
    //can with the js call, so we just set it to 1 million, but ideally we would
    //like to set both max and initial to the number of fns in all module and/or
    //use table.grow.
    env.fn_table = new WebAssembly.Table({initial:100 * 1000, maximum: 1000000, element:"anyfunc"});
    fs.writeFile("__data_end", "" + env.data_start,
        function(err) {
            if (err) return console.log(err);
        })
}

async function instantiate_module(env, { compiled_module, data_offset, data_size, fn_table_offset,
                                         namespace, globals}) {
    data_offset += env.data_start;
    fn_table_offset += env.fn_table_start;

    // console.log(globals);
    const wasm_globals = {};
    for (const [global, offset] of Object.entries(globals)) {
        // console.log(global, offset);
        wasm_globals[global] = new WebAssembly.Global({ value: 'i32', mutable: false },  offset);
    }
    // console.log(wasm_globals);

    // console.log("DATA_OFFSET: ", data_offset, globals.data_offset.value);
    let module_import_object = {
        env: Object.assign(
            {},
            wasm_globals,
            {   fn_table: env.fn_table,
                runtime_error: env.runtime_error_fn,
                stack_pointer: env.stack_pointer_global,
                data_offset: new WebAssembly.Global({ value: 'i32', mutable: false }, data_offset),
                fn_table_offset: new WebAssembly.Global({ value: 'i32', mutable: false }, fn_table_offset),

                log_string: makeLogString(env.runtime.memory),
                log_int: arg => console.log(arg),
                log_string_n: makeLogStringN(env.runtime.memory),
            },
            env.runtime.exports) };


    // console.log("From nodejs.js: data_offset: ", data_offset, " data_size: ",
    //             data_size, " data_end: ", data_offset + data_size, " fn_table_offset: ", fn_table_offset);
    console.log("Instantiating " + namespace);
    const instance =  await WebAssembly.instantiate(compiled_module, module_import_object).
        then(instance => instance);
    env.runtime.exports.rewrite_pointers(data_offset, data_size, fn_table_offset);
    env.modules[namespace].instance = instance;
    return instance;
}

async function instantiate_modules(env, modules) {
    for (const [namespace, module] of Object.entries(modules)) {
        module.instance = await instantiate_module(env, module);
    }
}

async function load_module(env, module) {
    console.log("Loading namespace: ", module.namespace);
    module.file_name =
        env.config.out + "/" +
        module.namespace
              .replace(/\./g,"/")
              .replace(/-/g, "_") +
        ".wasm";
    let buf = fs.readFileSync(module.file_name);

    module.compiled_module = await WebAssembly.compile(new Uint8Array(buf)).then(mod => mod);

    let module_info = parse_custom_sections(module.compiled_module);
    if (module_info.error) {
        console.log(module_info.error + " in module loaded from " + module.file_name);
        return module;
    }
    module = Object.assign(module, module_info);
    // console.log("MODULE_INFO:");
    // console.log(module);
    return module;
}


async function load_modules(env, module) {

    env.trace.push(module.namespace);
    let loaded_module = env.modules[module.namespace];
    if (loaded_module) return loaded_module;

    module = await load_module(env, module);

    module.data_offset = env.data_offset;
    module.fn_table_offset = env.fn_table_offset;
    env.data_offset += parseInt(module.data_size);
    env.fn_table_offset += parseInt(module.fn_table_size);

    for (const [namespace, value] of Object.entries(module.deps)) {
        if (env.trace.includes(namespace)) {
            console.warn("WARNING: Cyclical dependency from namespace " + module.namespace +
                         " to " + namespace);
            break;
        }
        await load_modules(env, { namespace: namespace });
    }

    env.modules[module.namespace] = module;
    env.trace.pop(module.namespace);

    return module;
}

function link_modules(env, modules) {
    for (const [namespace, module] of Object.entries(modules)) {
        const globals = {};

        for (const [namespace_dep, symbols] of Object.entries(module.deps)) {
            const module = env.modules[namespace_dep];
            for (const symbol of symbols) {
                if (!module) throw("Linking error in " + namespace + ": " + namespace_dep + " is not available");
                const symbol_info = module.symbol_table[symbol.name];
                if (!symbol_info) throw("Linking error in " + namespace + ": " + symbol.name +
                                        " does not exist in " + namespace_dep);
                const offset_type = symbol_info[symbol.offset_type];
                if (!offset_type) throw("Linking error in " + namespace + ": " +
                                        " Can't find offset type of " + symbol.offset_type +
                                        " in symbol info of " + symbol.name + " in namespace " + namespace_dep);
                globals[symbol.global] = parseInt(
                    symbol_info[symbol.offset_type]) +
                    module[symbol.offset] +
                    env[symbol.start];
            }
        }
        // console.log("GLOBALS: ", globals);
        module.globals = globals;
    }
}

async function start() {
    try {
        const env = {
            trace: [],
            modules: {},
            data_offset: 0,
            fn_table_offset: 0,
            config: {runtime_wasm: './out_wasm/runtime.wasm',
                     main: "main",
                     out: "./clj",
                     stack_size: 8 * 1024 }
        }

        console.log("Instantiate runtime ------------------------------------");
        await instantiate_runtime(env);

        console.log("data_end =", env.runtime.exports.__data_end.value);
        console.log("env.stack_pointer:", env.stack_pointer);
        console.log("heap_base =", env.runtime.exports.__heap_base.value);
        console.log("Load modules ------------------------------------");
        let module = { namespace: env.config.main };
        await load_modules(env, module);

        console.log("Link modules  ----------------------------------------");
        // console.log(util.inspect(env.modules, null, Infinity, true));
        link_modules(env, env.modules);

        console.log("Instantiate modules  ----------------------------------------");
        await instantiate_modules(env, env.modules);
        // console.log(util.inspect(env.modules, null, Infinity, true));

        const main_module = env.modules[env.config.main];

        console.log("Running main/test(arg1, arg2, ..) ------------------------------");
       
        run_wajure_fn(env, main_module, "main", 888, 777);

        env.runtime.exports.free_lispy_mempools();
        env.runtime.exports. free_malloc();

        console.log("End ------------------------------");
    } catch (e) {
        console.log(e);
    }

}


start();





// console.log("printf_ :", runtime.printf_);
// const __data_end = new WebAssembly.Global({value:'i32', mutable:false}, runtime.__data_end);
// let wajureImportObject = { env: { memory: runtime.memory,
//                                  printf_: runtime.printf_,
//                                  log_int: arg => console.log(arg),
//                                  log_string: makeLogString(runtime.memory),
//                                  log_string_n: makeLogStringN(runtime.memory),
//                                  make_lval_num: runtime.make_lval_num,
//                                  __data_end: runtime.__data_end
//                                }};

// makeLogStringN(wajure.mem)(runtime.__data_end.value , 4);
// makeLogString(wajure.mem)(runtime.__data_end.value);
// console.log("test: ", wajure.test);


// console.log("data_end =", runtime.__data_end.value);
// console.log("heap_base =", runtime.__heap_base.value);

// fs.writeFile("__heap_base", "" + runtime.__heap_base.value,
//              function (err) {
//                  if (err) return console.log(err);
//              })


    // console.log("data_end =", runtime.__data_end.value);
