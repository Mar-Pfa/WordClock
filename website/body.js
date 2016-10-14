ajax.load("jscolor.js", {Type:"insert"}, function() {
    updateDynamic(function () {
        RefreshUpdate();                
    });               
});   

function $(e) {
    return (typeof e==="string" ? document.getElementById(e) : e);
}

function changeApMode()
{
    if ($("apmode").checked)
    {
        $("ssid").style.display="";
        $("ssidlist").style.display="none"
    } 
    else
    {
        $("ssid").style.display="none";
        $("ssidlist").style.display=""
    }
}

function changeTime(component, value)
{
    var query="/serve/changeTime?component="+component+"&value="+value;
    ajax.loaddirect(query, function(result) {
        RefreshUpdate(function () {
            prepareDynamicUpdate();
        });
    });                    
}

var dynamicTimeOutVar;
function prepareDynamicUpdate()
{
    if (typeof(OutVar) !== 'undefined')
    {
        clearTimeout(dynamicTimeOutVar);
    }
    dynamicTimeOutVar = setTimeout(updateDynamic, 100);

}

var timeOutVar;
function prepareParameterUpdate()
{
    if (typeof(timeOutVar) !== 'undefined')
    { 
        clearTimeout(timeOutVar);
    }
    timeOutVar = setTimeout(callParameterUpdate, 100);                        
}

var lastColor;
function checkColor()
{
    if (typeof(lastColor) === 'undefined')
    {
        lastColor = $('color').value;
    } 
    if ($('color').value != lastColor)
    {
        lastColor = $('color').value;
        prepareParameterUpdate();
    }
}

function callParameterUpdate()
{
    var query = "/serve/update"+
        "?color="+encodeURIComponent("#"+$("color").value)+
        "&hue="+$("hue").value+
        "&dynamic="+$("dynamic").value+
        "&ssid="+$("ssid").value+
        "&password="+$("password").value+
        "&apmode="+$("apmode").checked+
        "&ssidlist="+$("ssidlist").value;
    ajax.loaddirect(query);
}

function SaveSettings()
{
    var query = "/serve/SaveSettings";
    ajax.loaddirect(query);
}

function RefreshClock(next)
{
    ajax.loaddirect('/serve/clock', function(result) {
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
    ajax.loaddirect('/serve/dynamic', function(result) {
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

window.setTimeout(function() {
    ajax.loaddirect("/serve/ssidlist", function(result) {
        var entries = result.split("\n");
        if (!entries)
            return;
        
        var ssidlist = $('ssidlist');        
        entries.forEach(function(entry) {
            if (entry != "")
            {
                var parts = entry.split("|");
                if (parts.length == 2)
                {
                    var option = document.createElement('option');

                    // Set the value using the item in the JSON array.
                    option.value = parts[0];
                    option.innerText = parts[1];
                    // Add the <option> element to the <datalist>.
                    ssidlist.appendChild(option);
                }
                else{
                    console.log("cannot parse string "+entry);
                }
            }        
        });        
    });
}, 10);

window.setInterval(checkColor,2000);

