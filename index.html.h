const char index_html[] PROGMEM = R"=====(<html>
    <head>        
        <title>Clock Website Admin</title>
        <link rel="stylesheet" href="milligram.min.css">
        <script src="microajax.js"></script>
    </head>
<body>    
    <form>
        <h3>Wifi Connection Settings:</h3>
        <fieldset>
            <label for="ssid">SSID:</label>
            <input type="text" placeholder="ssid" id="ssid">
            <label for="password">Password:</label>
            <input type="password" placeholder="password" id="password">
            <input class="button-primary" type="submit" value="Update Settings" onclick="storeSsid();return false;">
        </fieldset>
    </form>
    <pre id="clock"></pre>
    <script type="text/javascript">        
    	function $(e) {
			return (typeof e==="string" ? document.getElementById(e) : e);
		}

        function storeSsid()
        {
            var query = "/serve/update?ssid="+$("ssid").value+"&password="+$("password").value;
            microAjax(query, function() {});
        }

        function RefreshClock()
        {
            microAjax('/serve/clock', function(result) {
                $('clock').innerHTML = "";
                for(var x = 0, c=''; c = result.charAt(x); x++){ 
                    $('clock').innerHTML+=c;
                    if (x % 11 == 10)
                    {
                        $('clock').innerHTML+="\n";
                    } 
                }
            });            
        }    

        window.addEventListener('load', function() {
            microAjax('/serve/dynamic', function(result) {
                var entries = result.split("\n");
                if (entries)
                {
                    entries.forEach(function(entry) {
                        var parts = entry.split("|");
                        if (parts.length == 2)
                        {
                            $(parts[0]).value=parts[1];
                        }
                    });
                }
                RefreshClock();
                setInterval(function (){ RefreshClock(); },10000);                                
            });
        }, false);

    </script>
</body>
</html>)=====";