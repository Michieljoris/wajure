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
// "clj/main.wasm"
async function print_custom_section(filename, section_name) {
    let buf = fs.readFileSync(filename);
    module = await WebAssembly.compile(new Uint8Array(buf)).then(mod => mod);

    let deps = get_custom_section(module, section_name);
    // if (deps == undefined) return "Missing deps";
    // console.log(deps);
    console.log(deps);
}

print_custom_section(process.argv[2], process.argv[3]);
