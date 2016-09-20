# WordClock
Arduino Sketch with software for a wordclock controlled by a esp8266 chip and powered by a ws2812 led stripe

# clock.ino
is the adruino sketch with the firmware

# folder website
contains code for the administration website, based on simple html and a minimal ajax framework, created with node.js
the script "build.js" copies the website content into the rootfolder and convert them to c header files for static inclusion of the website content.

