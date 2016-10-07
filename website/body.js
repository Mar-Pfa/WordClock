ajax.load("jscolor.js", {Type:"insert"}, function() {
    updateDynamic(function () {
        RefreshUpdate();                
    });               
});   

function $(e) {
    return (typeof e==="string" ? document.getElementById(e) : e);
}

function changeTime(component, value)
{
    var query="/serve/changeTime?component="+component+"&value="+value;
    microAjax(query, function(result) {
        RefreshUpdate(function () {
            updateDynamic();
        });
    });                    
}

var timeOutVar;
function prepareParameterUpdate()
{
    if (typeof(timeOutVar) !== 'undefined')
    { 
        clearTimeout(timeOutVar);
    }
    timeOutVar = setTimeout(callParameterUpdate, 250);                        
}

var lastColor;
function checkColor()
{
    if (typeof(lastColor) === 'undefined' || $('color').value != lastColor)
    {
        lastColor = $('color').value;
        prepareParameterUpdate();
    }
}

function callParameterUpdate()
{
    var query = "/serve/update"+
        "?color="+encodeURIComponent($("color").value)+
        "&hue="+$("hue").value+
        "&dynamic="+$("dynamic").value+
        "&ssid="+$("ssid").value+
        "&password="+$("password").value;
    microAjax(query, function(result) {});
}

function SaveSettings()
{
    var query = "/serve/SaveSettings";
    microAjax(query, function() {});
}

function RefreshClock(next)
{
    microAjax('/serve/clock', function(result) {
        $('clock').innerHTML = "";
        var po = 0;
        console.log("check clock:");
        console.log(result);
        for (var x=0;x<110;x++)
        {        
            var c = result.charAt(x*8);
            var co = result.substring(x*8+1,x*8+8);
            $('clock').innerHTML+= "<span style='background-color:"+co+"'>"+c+"</span>";
            if (x % 11 == 10)
            {
                $('clock').innerHTML+="<br />";
            }
        }        
        /*
        for(var x = 0, c=''; c = result.charAt(x); x++){ 
            $('clock').innerHTML+=c;
            if (x % 11 == 10)
            {
                $('clock').innerHTML+="\n";
            } 
        }
        */

        if (typeof(next) !== "undefined")
        {
            next();
        }                
    });            
}


function updateDynamic(next)
{
    microAjax('/serve/dynamic', function(result) {

        var entries = result.split("\n");
        if (entries)
        {
            entries.forEach(function(entry) {
                var parts = entry.split("|");
                if (parts.length == 2)
                {
                    $(parts[0]).value=parts[1].trim();
                }                        
                if (parts.length == 3)
                {
                    switch(parts[2].trim())
                    {
                        case "text":
                            $(parts[0]).innerText = parts[1];
                            break;
                    }
                }
            });
        }

        if (typeof(next) !== "undefined")
        {
            next();
        }
    });
}

var refreshVar;
function RefreshUpdate(next)
{
    if (typeof(refreshVar) !== "undefined")
    {
        window.clearInterval(refreshVar);
    }
    RefreshClock(function() {                
        refreshVar = setInterval(function (){ RefreshClock(); },10000);
        if (typeof(next) !== "undefined")
        {
            next();
        }
    });                                                
}

window.setInterval(checkColor,5000);
