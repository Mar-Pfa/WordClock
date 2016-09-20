fs = require('fs');

var files = ["index.html" , "microajax.js" , "milligram.min.css" , "colors.js", "colorPicker.data.js", "colorPicker.js", "jsColor.js"];

files.forEach(function(file) {

    fs.readFile(file,function(err,data) {
        console.log("converting "+file);
        fs.writeFile("../"+file+".h",'const char '+file.split('.').join('_')+'[] PROGMEM = R"=====('+data+')=====";');
    });
});

   