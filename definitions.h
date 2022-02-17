#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define FW_BIN "http://www.djprime.de/IoT/clock"
#define FW_URL "http://www.djprime.de/IoT/clock.php?key=0tA1"

#define fallBackWifis 3
String fallBackWifiSsid[fallBackWifis] = { "My Own WLAN private section", "MagentaWLAN - YSJN", "DoWeiSteubi-Family+Friends" };
String fallBackWifiPassword[fallBackWifis] = { "KaLL3nd3r", "93814390029413929837", "B0xdOrf_Wifi" };

#endif

/* FW History
 * 1004 - 1006 add color change mode
 * 1003 add Siegsdorf Wifi
 * 1002 add working OTA Support
*/
