# ArduinoWebControl

This is just a prototype. It demonstrates how to control the pins of an Arduino board via a webinterface in order to set the color of an RGB led.

####Build

  `git clone --recursive https://github.com/philgras/ArduinoWebControl.git`<br>
  `mkdir _build`<br>
  `cd _build`<br>
  `cmake ..`<br>
  `make`<br>
  `/.ArduinoWebControl /dev/yourDevice`
  
Upload the sketch onto your Arduino. Then type `localhost:8000/index.html` into the browser's URL bar. It is important to turn of the favicon request from the browser.
  
####Arduino Setup

<img src=https://raw.githubusercontent.com/philgras/ArduinoWebControl/master/RGBLed.png width="350">
