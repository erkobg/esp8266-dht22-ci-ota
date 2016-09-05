# esp8266-dht22-ci-ota
**DHT22 with  Continuus Delivery by OTA**

 1. The build is tagged with a version number using PlatformIO build-flag
 
 2. A web server with an intermediate PHP script loads `version.json` file 
   to check the latest release version and returns this as a string. 
   
 3. ESP sends it’s current running firmware version to the PHP script using  
   tag argument `&tag=0.0.1`
      
 4. If the running firmware version does not match the latest release    version, the PHP script grabs the latest release `firmware.bin` from
   the same    web server and serves it up as a download to the ESP.
   
 5.  ESP then flashes the compiled `firmware.bin` OTA update using the    ESP8266httpUpdate library which is built-in to the espressif Arduino 
   tool-chain

**`firmware.bin`** is located `.pioenvs / board` folder.
In my case it is **`.pioenvs /d1_mini`** folder


Just copy the **`firmware.bin`** to the web server folder then update the file **`version.json`** with editor changing `"tag_name": "0.0.1",` for example to **`"tag_name": "0.0.2",`**
