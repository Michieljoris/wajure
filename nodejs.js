const fs = require('fs');

var str = "";

var memory;
var runtime;

var page_size = 64 * 1024;
var max_page_count = 10;
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
    TOO_MANY_ARGS: 1
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
    let stack_pointer = module.instance.exports.stack_pointer.value;
    // console.log("stack_pointer: ", stack_pointer);

    let args_count = args.length;

    var int32s = new Uint32Array(env.runtime.exports.memory.buffer, 0);
    stack_pointer /= 4;
    for (i = 0; i < args_count; i++) {
        let arg = env.runtime.exports.make_lval_num(args[i]);
        int32s[stack_pointer + i] = arg
    }
    module.instance.exports.stack_pointer.value += args_count * 4;

    let closure_pointer = 0;
    try {
        module.instance.exports[fn_name](closure_pointer, args_count);
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
        return string;
    };

}

function parse_symbol_info(a) {
    let result = { name: a[0],
                   ptr : a[1],
                   type: a[2],}
    if (result.type == "Function") {
        result.param_count = a[3];
        result.rest_arg_index = a[4];
    }
    return result;
}

function parse_custom_sections(module) {
    var nameSections = WebAssembly.Module.customSections(module, "symbol_table");

    let symbol_table = get_custom_section(module, "symbol_table");
    if (!symbol_table) return { error: "Missing symbol table"};

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
    // if (!deps) return { error: "Missing deps"};

    deps = deps.split("\n")
               .filter(l => l.length > 0)
               .map(l => l.split("/"))
               .map(a => { return { namespace: a[0], name: a[1] }})
               .reduce((acc, v) => {
                   acc[v.namespace] = acc[v.namespace] || [];
                   acc[v.namespace].push(v.name);
                   return acc;
               }, {})

    let data_size = get_custom_section(module, "data_size");
    if (!data_size) return { error: "Missing data_size"};

    return { symbol_table, deps, data_size };
}

async function load_module(env, wasm_path) {
    console.log("wasm_path: ", wasm_path)
    let buf = fs.readFileSync(wasm_path);
    var module = await WebAssembly.compile(new Uint8Array(buf)).then(mod => mod);

    let module_info = parse_custom_sections(module);
    if (module_info.error) {
        console.log(module_info.error + " in module loaded from " + wasm_path);
        return;
    }
    // console.log("MODULE_INFO:");
    // console.log(module_info);
    module_info.module = module;
    return module_info;
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
            grow_memory: () => { //console.log("grow_memory");
                if (++page_count > max_page_count) {
                    console.log("Error: can't allocate memory beyond max\n");
                    // throw "Error: can't allocate memory beyond max\n";
                    return 0;
                }
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

    runtime.exports.init_lispy_mempools(800, 800, 800);
    env.stack_pointer = stack_pointer;
    env.runtime = runtime;
    env.data_start = env.runtime.exports.__data_end.value;

    fs.writeFile("__data_end", "" + env.data_start,
        function(err) {
            if (err) return console.log(err);
        })
}

async function instantiate_module(env, { module, data_offset, data_size, fn_table_offset}) {
    data_offset += env.data_start;
    const globals = {
        stack_pointer: new WebAssembly.Global({ value: 'i32', mutable: true }, env.stack_pointer),
        data_offset: new WebAssembly.Global({ value: 'i32', mutable: false }, data_offset),
        fn_table_offset: new WebAssembly.Global({ value: 'i32', mutable: false }, fn_table_offset)
    }

    // console.log("DATA_OFFSET: ", data_offset, globals.data_offset.value);
    let module_import_object = {
        env: Object.assign(globals,
                           {
                               log_string: makeLogString(env.runtime.memory),
                               log_int: arg => console.log(arg),
                               log_string_n: makeLogStringN(env.runtime.memory),
                               runtime_error: make_runtime_error_fn(env.runtime.memory)
                           },
                           env.runtime.exports) };


    // console.log("From nodejs.js: data_offset: ", data_offset, " data_size: ",
    //             data_size, " data_end: ", data_offset + data_size, " fn_table_offset: ", fn_table_offset);

    const instance =  await WebAssembly.instantiate(module, module_import_object).
        then(instance => instance);
    env.runtime.exports.rewrite_pointers(data_offset, data_size, fn_table_offset);
    return instance;
}

async function start() {
    try {
        const env = {
            config: {runtime_wasm: './out_wasm/runtime.wasm',
                     main: "main",
                     out: "./compiled",
                     stack_size: 8 * 1024,
                    }
        }

        let main_wasm = env.config.out + "/" + env.config.main + ".wasm";

        console.log("Instantiating runtime ------------------------------------");
        await instantiate_runtime(env);

        // console.log("env.stack_pointer:", env.stack_pointer);
        console.log("Loading main module ------------------------------------");
        let main_module = await load_module(env, main_wasm);

        console.log("Instantiating main.wat ----------------------------------------");
        main_module.data_offset = 0;
        main_module.fn_table_offset = 0;
        main_module.instance = instance = await instantiate_module(env, main_module);

        console.log("Running main/test(..) ------------------------------");
        run_wajure_fn(env, main_module, "test", 888, 777);

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
