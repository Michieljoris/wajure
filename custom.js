const fs = require('fs');

function get_custom_section(module, section) {
    var nameSections = WebAssembly.Module.customSections(module, section);

    if (nameSections.length != 0) {
        // console.log("Module contains a custom section: ", section);
        var string = new TextDecoder('utf8').decode(nameSections[0]);
        // console.log(string);
        return string;
    };

}

async function print_custom_section() {
    let buf = fs.readFileSync("clj/main.wasm");
    module = await WebAssembly.compile(new Uint8Array(buf)).then(mod => mod);

    let deps = get_custom_section(module, "deps");
    // if (deps == undefined) return "Missing deps";
    // console.log(deps);
    console.log(deps);
}

print_custom_section(process.argv[2]);
