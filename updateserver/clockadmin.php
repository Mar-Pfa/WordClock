<!DOCTYPE html>
<html>
    <head>     
       	<meta charset='utf-8'>   
        <title>Clock Website Admin</title>        
        <script type="text/javascript" src="ajax.js" ></script>
    </head>
<body> 

<h1>List of Devices out there</h1>
<table>
<tr><td>Devicename</td><td>Firmware Version</td><td>Ssid</td><td>Lastcontact</td><td>Link</td></tr>
<?php

$filename = "clocks/clocks.txt";
$lines = explode("\n",file_get_contents($filename));
$found = false;
$linesNew = array();


foreach($lines as $line)
{
    if ($line=="null")
        continue;
    if ($line=="")
        continue;
    $clock = json_decode($line);
    echo "<tr><td>$clock->devicename</td><td>$clock->fwVersion</td><td>$clock->ssid</td><td>$clock->lastcontact</td><td><a href='http://$clock->localip' target='_blank'>Open Management Page</a></td></tr>";
}
?>
</table>
</body>
</html>
