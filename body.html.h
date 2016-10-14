const char body_html[] PROGMEM = R"=====(    <form>
        <h3>Wifi Connection Settings:</h3>
        <fieldset>

            <label for="apcheckbox">Access Point Mode:</label>
            <input type="checkbox" id="apmode" value="1" onchange="changeApMode();return false;" />
            <label for="ssid">SSID:</label>
            <input type="text"  style="display:none" placeholder="ssid" id="ssid" oninput = "prepareParameterUpdate();return false;">

            <select id="ssidlist" onchange="prepareParameterUpdate();return false;" >
                <option value="">please select...</option>
            </select>

            <label for="password">Password:</label>
            <input type="password" placeholder="password" id="password" oninput = "prepareParameterUpdate();return false;"/>            
            <label for="color">Farbe</label> 
	        <!--<input class="color" style="width:100%" class="jscolor" value="" id="color" onchange="prepareParameterUpdate();return false;" oninput = "prepareParameterUpdate();return false;" /><br />-->
            <input class="jscolor" id="color"  style="width:100%" value="ff0000" onchange="prepareParameterUpdate();return false;" oninput = "prepareParameterUpdate();return false;"/>
            <table style="width:auto">
                <tr><td><button id="hourplus" onclick="changeTime('hour',1);return false;">+</button></td><td></td><td><button  onclick="changeTime('minute',1);return false;"id="minuteplus">+</button></td></tr>
                <tr><td style="text-align:center"><label id="_hour">12</label></td><td>:</td><td style="text-align:center"><label id="_minute">23</label></td></tr>
                <tr><td><button id="hourminus"  onclick="changeTime('hour',-1);return false;">-</button></td><td></td><td><button id="minuteminus"  onclick="changeTime('minute',-1);return false;">-</button></td></tr>
            </table>  
            <label for="hue">Helligkeit:</label>
            <input type="range" id="hue" min="0" max="255" oninput="prepareParameterUpdate();return false;">
            <p style="font-style:italic;white-space:nowrap">Umgebungshelligkeit: <span id="envhue">0</span></p>
            <label for="dynamic">Lichtdynamik:</label>
            <input type="range" id="dynamic" min="0" max="255" oninput="prepareParameterUpdate();return false;">
            <br />
            <input class="button-primary" type="submit" value="Update Settings" onclick="SaveSettings();return false;" />
        </fieldset>
    </form>
    <pre id="clock"></pre>
)=====";