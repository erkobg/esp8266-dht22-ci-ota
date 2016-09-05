#include "DHT.h"
//#include <ArduinoOTA.h>
#include "ThingSpeak.h"
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266httpUpdate.h>

// Stringifying the BUILD_TAG parameter in platformio.ini
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String buildTag = ESCAPEQUOTE(BUILD_TAG);

#define DEEP_SLEEP_SECONDS   240   //240 = 2 min
#define DHTPIN D4    // what digital pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define HOSTNAME "ESP_OTA_"
const char * _AP_name ="YOUR_AP_NAME";
const char * _AP_pass ="SOME_PASSWORD";
WiFiClient client;
//Thingspeak Settings
unsigned long THINGSPEAK_CHANNEL_ID = CHANNEL_ID;
const char * THINGSPEAK_API_WRITE_KEY = "CHANNEL_WRITE_KEY";


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
void configModeCallback (WiFiManager *myWiFiManager);


void setup() {
        Serial.begin(115200);
        Serial.println("DHTxx test!");
        //WiFiManager
        //Local intialization. Once its business is done, there is no need to keep it around
        WiFiManager wifiManager;
        // Uncomment for testing wifi manager
        //wifiManager.resetSettings();
        wifiManager.setAPCallback(configModeCallback);

        //or use this for auto generated name ESP + ChipID
        wifiManager.autoConnect(_AP_name,_AP_pass);

        //Manual Wifi
        //WiFi.begin(WIFI_SSID, WIFI_PWD);
        String hostname(HOSTNAME);
        hostname += String(ESP.getChipId(), HEX);
        WiFi.hostname(hostname);


        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");

        }
        // Setup OTA
        Serial.println("Hostname: " + hostname);
        // ArduinoOTA.setHostname((const char *)hostname.c_str());
        // ArduinoOTA.begin();

        ThingSpeak.begin(client);

        dht.begin();
}

void loop() {
        Serial.println("Started..");
        if((WiFi.status() == WL_CONNECTED)) {

                Serial.println("Checking for Update. Current version: " + buildTag);
                t_httpUpdate_return ret = ESPhttpUpdate.update("http://WEBSERVER_IP/ota_update/firmware.php?tag=" + buildTag);

                switch(ret) {
                case HTTP_UPDATE_FAILED:
                        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                        break;

                case HTTP_UPDATE_NO_UPDATES:
                        Serial.println("HTTP_UPDATE_NO_UPDATES");
                        break;

                case HTTP_UPDATE_OK:
                        Serial.println("HTTP_UPDATE_OK");
                        break;
                }
        }

        // ArduinoOTA.handle();

        // Wait a few seconds between measurements.
      //  delay(2000);

        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.readTemperature();

        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t) ) {
                Serial.println("Failed to read from DHT sensor!");
                return;
        }

        // Compute heat index in Celsius (isFahreheit = false)
        float hic = dht.computeHeatIndex(t, h, false);

        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" *C\t ");
        Serial.print("Heat index: ");
        Serial.print(hic);
        Serial.println(" *C ");
        Serial.println( "Updating thingspeak...");
        ThingSpeak.setField(1,t);
        ThingSpeak.setField(2,h);
        ThingSpeak.setField(3,hic);
        //  thingspeak.getLastChannelItem(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_WRITE_KEY);
        ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_WRITE_KEY);
        Serial.println( "Updating thingspeak...done");

  #ifdef DEEP_SLEEP_SECONDS
        // Enter DeepSleep
        Serial.println(F("Sleeping..."));
        ESP.deepSleep(DEEP_SLEEP_SECONDS * 1000000, WAKE_RF_DEFAULT);
        // Do nothing while we wait for sleep to overcome us
        while(true) {};
#endif
}
void configModeCallback (WiFiManager *myWiFiManager) {
        Serial.println("Entered config mode");
        Serial.println(WiFi.softAPIP());
        //if you used auto generated SSID, print it
        Serial.println(myWiFiManager->getConfigPortalSSID());
}
