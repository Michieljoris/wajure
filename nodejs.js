const util = require('util');
const fs = require('fs');

var initial_page_count = 10;
var page_size = 64 * 1024;
var max_page_count = 100;
var initial_memory_size = initial_page_count * page_size;
var page_count = initial_page_count;

function pad(s) {
    return (s % 4) ? (s + 4 - (s % 4)) : s;
}

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
    WRONG_NUMBER_OF_ARGS: 0,
    NOT_A_FN: 1
}

const m = { "1": 123 };

const lval_types_to_str = {
    "5": "nil",
    "6":  "true",
    "7": "false",
    "8": "number",
    "9": "string",
    "10": "regex",
    "11": "keyword",
    "12": "symbol",
    "13": "unbound",
    // collection subtypes
    "18": "list",
    "19": "map",
    "20": "vector",
    "21": "set",
}

function offset_to_str(offset) {
    var string = "";
    if (offset) {
        var bytes = new Uint8Array(memory.buffer, offset);
        var length = 0;
        while (bytes[length] != 0) length++;
        var bytes = new Uint8Array(memory.buffer, offset, length);
        string = new TextDecoder('utf8').decode(bytes);
    }
    return string;
}

function make_runtime_error_fn(memory, offset) {
    return function(err_no, offset) {
        switch (err_no) {
            case runtime_error_codes.NOT_A_FN:
                // string = "Not a fn: " + offset; break;
                let type = lval_types_to_str[offset];
                type = type ? type : offset;
                string = "A " + type + " cannot be used as a function";
                break;
            case runtime_error_codes.WRONG_NUMBER_OF_ARGS:
                string = "Wrong number of args passed." +
                    offset_to_str(offset); break;
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
    let args_count = args.length;
    let args_block_ptr = env.runtime.exports.lalloc_size(4);
    let ints = new Uint32Array(env.memory.buffer, args_block_ptr, args_count * 4);
    for (i = 0;  i < args_count; i++) {
        let arg = env.runtime.exports.make_lval_num(args[i]);
        ints[i] = arg;
    }

    let closure_pointer = 0;
    try {
        console.log("Running: ", fn_name);
        console.log(module.instance.exports[fn_name](closure_pointer, args_block_ptr, args_count));
    } catch (e) {
        console.log("RUNTIME ERROR");
        console.log(e);
    }

}

async function run_wajure_fn2(env, module, fn_name, ...args) {
    let args_count = args.length;
    let wajure_args = [];
    for (i = 0;  i < args_count; i++) {
        let arg = env.runtime.exports.make_lval_num(args[i]);
        wajure_args[i] = arg;
    }

    let closure_pointer = 0;
    try {
        console.log("Running: ", fn_name);
        console.log(module.instance.exports[fn_name](closure_pointer, ... wajure_args));
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

    return { symbol_table, deps,
             data_size: parseInt(data_size), fn_table_size: parseInt(fn_table_size) };
}

async function instantiate_runtime(env) {
    let { config } = env;
    let buf = fs.readFileSync(config.runtime_wasm);
    let  runtime;
    console.log("Creating wasm memory: " + (initial_page_count * page_size / 1024) + "kb")
    env.memory = new WebAssembly.Memory({initial:initial_page_count, maximum:max_page_count});
    let importObject = {
        env: {
            memory: env.memory,
            _putchar: arg => {
                process.stdout.write(String.fromCharCode(arg));
            } ,
            get_memory: () => {
                return env.heap_base;
            },
            grow_memory: () => {
                if (page_count + 1 > max_page_count) {
                    console.log("Error: can't allocate memory beyond max (nodejs) of " +
                                max_page_count * page_size / 1024 + "kb\n");
                    // throw "Error: can't allocate memory beyond max\n";
                    return 0;
                }
                console.log("Grow_memory from ", (page_count * page_size)/1024, "kb to ",
                            ((page_count+1) * page_size)/1024 , "kb");
                page_count++;
                // memory = runtime.exports.memory.grow(1);
                env.memory.grow(1);
                return 1;
            },
            get_mem_end: () => page_count * page_size
        }
    };
    runtime = await WebAssembly.instantiate(new Uint8Array(buf), importObject).
        then(res => res.instance);

    env.runtime = runtime;
    env.data_start = runtime.exports.__data_end.value;
    if (env.data_start !== runtime.exports.__heap_base.value) {
        //Memory layout should be: runtime stack|runtime data|wajure data|wajure heap
        throw("Error: runtime uses wrong memory layout, __data_end should equal __heap_base."+
              "Probably --stack-first flag is not set for linker.");
    }
    env.data_start = pad(env.data_start);

    env.fn_table_start = 0;

    // env.runtime_error_fn = make_runtime_error_fn(env.runtime.exports.memory);
    env.runtime_error_fn = make_runtime_error_fn(env.memory);
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
                                         namespace, file_name,  globals}) {
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
            {   memory: env.memory,
                fn_table: env.fn_table,
                runtime_error: env.runtime_error_fn,
                data_offset: new WebAssembly.Global({ value: 'i32', mutable: false }, data_offset),
                fn_table_offset: new WebAssembly.Global({ value: 'i32', mutable: false }, fn_table_offset),

                log_string: makeLogString(env.memory),
                log_int: arg => console.log(arg),
                log_string_n: makeLogStringN(env.memory),
                
            },
            wasm_globals,
            env.runtime.exports) };


    // console.log("From nodejs.js: data_offset: ", data_offset, " data_size: ",
    //             data_size, " data_end: ", data_offset + data_size, " fn_table_offset: ", fn_table_offset);
    console.log("Instantiating " + (namespace || file_name));
    const instance =  await WebAssembly.instantiate(compiled_module, module_import_object).
        then(instance => instance);
    env.runtime.exports.rewrite_pointers(data_offset, data_size, fn_table_offset);
    return instance;
}

async function instantiate_modules(env, modules) {
    for (const [namespace, module] of Object.entries(modules)) {
        module.instance = await instantiate_module(env, module);
    }
}

async function load_module(env, module) {
    if (module.namespace)
        console.log("Loading namespace: ", module.namespace);
    else
        console.log("Loading file: ", module.file_name);
   
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

async function instantiate_builtin(env) {
    let module = { file_name: env.config.builtin_wasm};
    await load_module(env, module);

    module.data_offset = 0;
    module.fn_table_offset = 0;

    env.data_offset = pad(module.data_size);
    env.fn_table_offset = module.fn_table_size;

    module.globals = {};
    module.instance = await instantiate_module(env, module);
    // console.log(module);
}

async function load_modules(env, module) {

    env.trace.push(module.namespace);
    let loaded_module = env.modules[module.namespace];
    if (loaded_module) return loaded_module;

    module.file_name =
        env.config.out + "/" +
        module.namespace
              .replace(/\./g,"/")
              .replace(/-/g, "_") +
        ".wasm";

    module = await load_module(env, module);

    module.data_offset = env.data_offset;
    module.fn_table_offset = env.fn_table_offset;

    env.data_offset += pad(module.data_size);
    env.fn_table_offset += module.fn_table_size;

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
            if (!module) throw("Linking error in " + namespace + ": " + namespace_dep + " is not available");
            for (const symbol of symbols) {
                const symbol_info = module.symbol_table[symbol.name];
                if (!symbol_info) throw("Linking error in " + namespace + ": " + symbol.name +
                                        " does not exist in " + namespace_dep);
                const offset = symbol_info[symbol.offset_type];
                if (!offset) throw("Linking error in " + namespace + ": " +
                                   " Can't find offset type of " + symbol.offset_type +
                                   " in symbol info of " + symbol.name + " in namespace " + namespace_dep);
                globals[symbol.global] = parseInt(offset) + module[symbol.offset] + env[symbol.start];

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
                     builtin_wasm: './out_wasm/builtin.wasm',
                     main: "main",
                     out: "./clj"}
        }

        console.log("Instantiate runtime ------------------------------------");
        await instantiate_runtime(env);
        await instantiate_builtin(env);
        // console.log("data_end =", env.runtime.exports.__data_end.value);

        console.log("Load modules ------------------------------------");
        let module = { namespace: env.config.main };
        await load_modules(env, module);
        env.heap_base = env.data_start + env.data_offset;


        if (env.heap_base >= initial_memory_size) {
            let new_page_count = Math.ceil(env.heap_base / page_size)
            console.log("NOTE: Inital memory (" + page_count * page_size / 1024  +
                        "kb, " + page_count +" pages) is set too low, there's not enough " +
                  "memory for the runtime stack +" + " runtime data + wajure data (total: "
                        + Math.ceil(env.heap_base / 1024) + "kb, "  + env.heap_base + " bytes).\nAdjust initial_page_count " +
                  "in this nodejs.js file to at least " + new_page_count + " pages, growing memory for now...");
            let extra_page_count = new_page_count - page_count;
            env.memory.grow(extra_page_count);
            page_count = new_page_count;
            console.log("New wasm memory size: " + (page_count * page_size / 1024) + "kb")

        }
        console.log("data_start =", env.data_start);
        console.log("heap_base =", env.heap_base);

        console.log("Link modules  ----------------------------------------");
        // console.log(util.inspect(env.modules, null, Infinity, true));
        link_modules(env, env.modules);

        console.log("Instantiate modules  ----------------------------------------");
        await instantiate_modules(env, env.modules);
        // console.log(util.inspect(env.modules, null, Infinity, true));
        const main_module = env.modules[env.config.main];

        console.log("Init runtime  ----------------------------------------");
        env.runtime.exports.init_malloc();
        env.runtime.exports.init_lispy_mempools(3200, 3200, 3200);
       
        console.log("Running main/main(arg1, arg2, ..) ------------------------------");
        run_wajure_fn(env, main_module, "main",1,2);

        env.runtime.exports.free_lispy_mempools();
        env.runtime.exports. free_malloc();

        console.log("------------------------------\nEnd");
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



    // console.log("data_end =", runtime.__data_end.value);

    // fs.writeFile("__heap_base", "" + runtime.exports.__heap_base.value,
    //              function (err) {
    //                  if (err) return console.log(err);})
