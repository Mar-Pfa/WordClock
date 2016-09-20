fs = require('fs');

var files = ["index.html" , "microajax.js" , "milligram.min.css" ];

files.forEach(function(file) {

    fs.readFile(file,function(err,data) {
        fs.writeFile("../"+file+".h",'const char '+file.split('.').join('_')+'[] PROGMEM = R"=====('+data+')=====";');
    });
});

