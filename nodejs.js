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

var importObject = {
    env: { _putchar: arg => {
        // if (arg == 10) { console.log(str); str = ""; }
        // else str = str + String.fromCharCode(arg);
        process.stdout.write(String.fromCharCode(arg))
    },
           grow_memory: () => { //console.log("grow_memory");
               if (++page_count > max_page_count) {
                   console.log("Error: can't allocate memory beyond max\n");
                   // throw "Error: can't allocate memory beyond max\n";
                   return 0;
               }
               memory = runtime.memory.grow(1);
               return 1; },
           get_mem_end:  () => { //console.log("get_mem_end");
               return page_count * page_size},
           // memory:  new WebAssembly.Memory({initial: initial_page_count,
           //                                  maximum: max_page_count})
         }
};

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

async function run_lispy_fn(fn_name, ...args) {
    const lispy = this;
    let stack_pointer = lispy.user.stack_pointer.value;
    // console.log("stack_pointer: ", stack_pointer);

    let args_count = args.length;

    var int32s = new Uint32Array(runtime.memory.buffer, 0);
    stack_pointer /= 4;
    for (i = 0; i < args_count; i++) {
        let arg = lispy.runtime.make_lval_num(args[i]);
        int32s[stack_pointer + i] = arg
    }
    lispy.user.stack_pointer.value += args_count * 4;

    let closure_pointer = 0;
    try {
    lispy.user[fn_name](closure_pointer, args_count);
    } catch (e) {
        console.log("RUNTIME ERROR");
        console.log(e);
    }

    lispy.runtime.free_lispy_mempools();
    lispy.runtime.free_malloc();

}

var data_size;

async function load_lispy(runtime, globals, lispy_wasm_file_name) {

    let buf = fs.readFileSync(lispy_wasm_file_name);
    let lispyImportObject = {env: Object.assign(globals,
                                                {log_string: makeLogString(runtime.memory),
                                                 log_int: arg => console.log(arg),
                                                 log_string_n: makeLogStringN(runtime.memory),
                                                 runtime_error: make_runtime_error_fn(runtime.memory)},
                                                runtime)};

    var module = await WebAssembly.compile(new Uint8Array(buf)).then(mod => mod);
    var nameSections = WebAssembly.Module.customSections(module, "symbol_table");

    if (nameSections.length != 0) {
        console.log("Module contains a custom section: symbol_table");
        var string = new TextDecoder('utf8').decode(nameSections[0]);
        console.log(string);
    };

    var nameSections = WebAssembly.Module.customSections(module, "data_size");

    if (nameSections.length != 0) {
        console.log("Module contains a custom section: data_size");
        var string = new TextDecoder('utf8').decode(nameSections[0]);
        console.log(parseInt(string));
        data_size = parseInt(string);
    };

    let lispy = await WebAssembly.instantiate(new Uint8Array(buf), lispyImportObject).
        then(res => { return res.instance.exports });
    return lispy;
}

async function load_runtime(runtime_wasm_file_name) {
    let buf = fs.readFileSync(runtime_wasm_file_name);
    runtime = await WebAssembly.instantiate(new Uint8Array(buf), importObject).
        then(res => res.instance.exports);

    runtime = await WebAssembly.instantiate(new Uint8Array(buf), importObject).
        then(res => res.instance.exports);

    fs.writeFile("__data_end", "" + runtime.__data_end.value,
                 function (err) {
                     if (err) return console.log(err);
                 })

    console.log("data_end =", runtime.__data_end.value);
    runtime.init_malloc();
    return runtime;
}

async function init_lispy({runtime_wasm_file_name, lispy_wasm_file_name, stack_size}) {

    console.log("Loading lispy runtime ------------------------------------");
    const runtime = await load_runtime('./out_wasm/runtime.wasm');

    let stack_pointer = runtime._malloc(stack_size); //8MB stack
    console.log("stack_pointer:", stack_pointer);
    runtime.init_lispy_mempools(800, 800, 800);

    const globals = {
        stack_pointer: new WebAssembly.Global({value:'i32', mutable:true}, stack_pointer),
        data_offset: new WebAssembly.Global({value:'i32', mutable:true}, 0),
        fn_table_offset: new WebAssembly.Global({value:'i32', mutable:true}, 0)
    }

    console.log("Loading lispy.wat ----------------------------------------");
    const user = await load_lispy(runtime, globals, './compiled/lispy.wasm');

    const run = run_lispy_fn;
    return {runtime_wasm_file_name, lispy_wasm_file_name, stack_size, user, runtime, run};
}

async function start() {
    try {
        const lispy_config = { runtime_wasm_file_name:  './out_wasm/runtime.wasm',
                               lispy_wasm_file_name: './compiled/lispy.wasm',
                               stack_size: 8 * 1024
                             }

        const lispy = await init_lispy(lispy_config);
        console.log("Running lispy.test ------------------------------");
        lispy.run("test", 888, 777);
        const data_start = lispy.runtime.__data_end.value;
        console.log("data_end: ", data_start, " data_size: ", data_size, " data_end: ", data_start+ data_size);
        lispy.runtime.rewrite_pointers(lispy.runtime.__data_end.value, data_size, 0);

        console.log("End ------------------------------");
    } catch(e) {
        console.log(e);
    }

}


start();





// console.log("printf_ :", runtime.printf_);
// const __data_end = new WebAssembly.Global({value:'i32', mutable:false}, runtime.__data_end);
// let lispyImportObject = { env: { memory: runtime.memory,
//                                  printf_: runtime.printf_,
//                                  log_int: arg => console.log(arg),
//                                  log_string: makeLogString(runtime.memory),
//                                  log_string_n: makeLogStringN(runtime.memory),
//                                  make_lval_num: runtime.make_lval_num,
//                                  __data_end: runtime.__data_end
//                                }};

// makeLogStringN(lispy.mem)(runtime.__data_end.value , 4);
// makeLogString(lispy.mem)(runtime.__data_end.value);
// console.log("test: ", lispy.test);


// console.log("data_end =", runtime.__data_end.value);
// console.log("heap_base =", runtime.__heap_base.value);

// fs.writeFile("__heap_base", "" + runtime.__heap_base.value,
//              function (err) {
//                  if (err) return console.log(err);
//              })
