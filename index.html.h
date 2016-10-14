const char index_html[] PROGMEM = R"=====(<!DOCTYPE html>
<html>
    <head>     
       	<meta charset='utf-8'>   
        <title>Clock Website Admin</title>        
        <script type="text/javascript" src="ajax.js" ></script>
    </head>
<body> 
        <div id="body" />
        <script type="text/javascript">
            window.onload = function () {
                ajax.load("milligram.min.css", {Type:"insert"}, function() {
                ajax.load("body.html",  {Type:"replace", Id:"body", Tag:"innerHTML"},function(data) {                
                ajax.load("body.js", {Type:"insert"}, function () {
                    //
                });                    
                });
                });
            }            
        </script>       
</body>
</html>)=====";