var express = require('express');
var app = express();

app.use(function (req, res, next) {
  console.log('Time:', new Date().toISOString()+" - "+req.originalUrl);
  next();
});
app.use(express.static(__dirname));

/*
app.get('/', function (req, res) {
  res.send('Hello World!');
});
*/

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});