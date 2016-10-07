var express = require('express');
var fs = require('fs');
var app = express();

app.use(function (req, res, next) {
  console.log(req.originalUrl);
  next();
});

app.get('/load', function (req, res) {
    
    fs.readFile(req.query.name, 'utf8', function(err, data) {
        if (err)
        {
            res.send(err);
            return;
        }
        if (req.query.start)
        {
            data = data.substring(req.query.start);
        }
        if (data.length>4000)
        {
            data = data.substring(0, 4000);
        }        
        res.send(data);
    });          
})

app.use(express.static('.'));

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});