var fs = require('fs');
var UglifyJS = require("uglify-js");

String.prototype.replaceAll = function(search, replacement) {
    var target = this;
    return target.replace(new RegExp(search, 'g'), replacement);
};

var files = ["index.html" , "milligram.min.css" , "jscolor.js", "ajax.js","body.js","body.html"];

console.log("add to c-code: ");
files.forEach(function(file) {
    var varName = file.split('.').join('_');
   console.log("#include \""+file +".h\";");
});

console.log("default action: ");
files.forEach(function(file) {
    var varName = file.split('.').join('_');
   console.log("server.on ( \"/"+file+"\", []() { server.send ( 200, \"text/html\", "+varName+" );  } );");
});


files.forEach(function(file) {

    fs.readFile(file,function(err,data) {
        var varName = file.split('.').join('_');
        data = data.toString();
        var output = data;                
        console.log("converting "+file+" with "+((output.length)/(1024)).toFixed(1)+"K");        
        if (file.endsWith(".js"))
        {
            output = UglifyJS.minify(file).code;
        }
        
        if (output.length > 1024+512) {
            console.log("warning - file size to big! "+file+": "+((output.length)/(1024)).toFixed(1)+"K");
        } else {
            console.log("new size: "+((output.length)/(1024)).toFixed(1)+"K");
        }
        
        var output = 'const char '+varName+'[] PROGMEM = R"=====('+output+')=====";';
        fs.writeFileSync("../website/"+file+".h",output, 'utf-8');
        //data = data.replaceAll('"','\\\"');
        //fs.writeFile("../"+file+".h",'const char* '+varName+' ="'+data+'";');        
    });
});


   