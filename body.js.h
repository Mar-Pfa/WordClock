const char body_js[] PROGMEM = R"=====(function $(e){return"string"==typeof e?document.getElementById(e):e}function changeApMode(){$("apmode").checked?($("ssid").style.display="",$("ssidlist").style.display="none"):($("ssid").style.display="none",$("ssidlist").style.display="")}function changeTime(e,a){var t="/serve/changeTime?component="+e+"&value="+a;ajax.loaddirect(t,function(e){RefreshUpdate(function(){prepareDynamicUpdate()})})}function prepareDynamicUpdate(){"undefined"!=typeof OutVar&&clearTimeout(dynamicTimeOutVar),dynamicTimeOutVar=setTimeout(updateDynamic,100)}function prepareParameterUpdate(){void 0!==timeOutVar&&clearTimeout(timeOutVar),timeOutVar=setTimeout(callParameterUpdate,100)}function checkColor(){void 0===lastColor&&(lastColor=$("color").value),$("color").value!=lastColor&&(lastColor=$("color").value,prepareParameterUpdate())}function callParameterUpdate(){var e="/serve/update?color="+encodeURIComponent("#"+$("color").value)+"&devicename="+$("devicename").value+"&hue="+$("hue").value+"&dynamic="+$("dynamic").value+"&ssid="+$("ssid").value+"&password="+$("password").value+"&apmode="+$("apmode").checked+"&ssidlist="+$("ssidlist").value;ajax.loaddirect(e)}function SaveSettings(){ajax.loaddirect("/serve/SaveSettings")}function RefreshClock(e){ajax.loaddirect("/serve/clock",function(a){$("clock").innerHTML="";console.log("check clock:"),console.log(a);for(var t=0;t<110;t++){var n=a.charAt(8*t),o=a.substring(8*t+1,8*t+8);$("clock").innerHTML+="<span style='background-color:"+o+"'>"+n+"</span>",t%11==10&&($("clock").innerHTML+="<br />")}void 0!==e&&e()})}function allOn(){ajax.loaddirect("serve/allon",function(e){console.log("all lights on")})}function updateDynamic(e){ajax.loaddirect("/serve/dynamic",function(a){var t=a.split("\n");t&&t.forEach(function(e){var a=e.split("|");if(2==a.length&&($(a[0]).value=a[1].trim()),3==a.length)switch(a[2].trim()){case"text":$(a[0]).innerText=a[1]}}),void 0!==e&&e()})}function RefreshUpdate(e){void 0!==refreshVar&&window.clearInterval(refreshVar),RefreshClock(function(){refreshVar=setInterval(function(){RefreshClock()},1e4),void 0!==e&&e()})}ajax.load("jscolor.js",{Type:"insert"},function(){updateDynamic(function(){RefreshUpdate()})});var dynamicTimeOutVar,timeOutVar,lastColor,refreshVar;window.setTimeout(function(){ajax.loaddirect("/serve/ssidlist",function(e){var a=e.split("\n");if(a){var t=$("ssidlist");a.forEach(function(e){if(""!=e){var a=e.split("|");if(2==a.length){var n=document.createElement("option");n.value=a[0],n.innerText=a[1],t.appendChild(n)}else console.log("cannot parse string "+e)}})}})},10),window.setInterval(checkColor,2e3);)=====";