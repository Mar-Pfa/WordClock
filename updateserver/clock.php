<?php
include "definitions.php";

function updateDevice($devicename, $mac, $fwVersion, $ssid, $password, $localip)
{
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
        if ($clock->mac == $mac)
        {
            // update device
            $found = true;
            $clock->devicename = $devicename;
            $clock->fwVersion = $fwVersion;
            $clock->ssid = $ssid;
            $clock->password = $password;
            $clock->lastcontact = date(DATE_ATOM, time());                        
            $clock->localip = $localip;
            array_push($linesNew, json_encode($clock));
        } else {
            // copy old value
            array_push($linesNew, $line);
        }
    }
    if ($found == false)
    {
        $clock = (object)[];
        $clock->mac = $mac;
        $clock->fwVersion = $fwVersion;
        $clock->lastcontact = date(DATE_ATOM, time());
        $clock->devicename = $devicename;
        $clock->ssid = $ssid;
        $clock->password = $password;
        $clock->localip = $localip;
        array_push($linesNew, json_encode($clock));    
    }
    $content = implode("\n",$linesNew);
    file_put_contents($filename, $content);
}


header("Access-Control-Allow-Origin: *");
if ($_GET["key"]==$ServerSharedKey)
{
    $devicename = $_GET["devicename"];
    $mac = $_GET["mac"];
    $fwVersion = $_GET["fwVersion"];
    $ssid = $_GET["ssid"];
    $password  = $_GET["password"];
    $localip = $_GET["s_ip"];
    $filename = "clockinfo.txt";
     
    if ($mac != null)
    {
        updateDevice($devicename, $mac, $fwVersion, $ssid, $password, $localip);
    }
    echo file_get_contents("clockLatestFW.txt");
}

?>