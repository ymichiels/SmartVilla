/*This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

 /********************
 * Program:  DHT22 humidity-temperature sensor tester,
 *           DHT11 humidity-temperature sensor tester,
 *           HC-SR04 sensor tester
 *           LDR sensor Tester
 *           PIR sensor tester
 * Description: print humidity and temperature to serial
 *              print distance to serial
 *              print motion movement
 *              print light boolean
 *              destinate to ESP8266 and ESP32 MCU
 ********************/

//Constants

//Sensor
//#define DHT_SENSOR
//#define DISTANCE_SENSOR
//#define LIGHT_SENSOR
//#define MOTION_SENSOR

#include"Arduino.h"

#ifdef WIFI_USED
    #ifdef ARDUINO_ARCH_ESP8266
        #include <ESP8266WiFi.h>
    #endif

    #ifdef ARDUINO_ARCH_ESP32
        #include <WiFi.h>
    #endif

    #include <PubSubClient.h>
    WiFiClient espClient;
    PubSubClient client{MQTT_IP, MQTT_PORT, espClient};

    #ifdef GEOLOCATION_USED
        #include <WifiLocation.h>
        WifiLocation location(API_KEY);
        #define TOPIC_LATITUDE "lat"
        #define TOPIC_LONGITUDE "lon"
    #endif
#endif

#ifdef DHT_PIN
    #include <DHT.h>
    #define TOPIC_HUM "hum"
    #define TOPIC_TEMP "temp"
    //#define DHT_TYPE DHT22    // DHT 22  (AM2302)
    #define DHT_TYPE DHT11      // DHT 11
    DHT dht(DHT_PIN, DHT_TYPE); // Initialize DHT sensor for normal 16mhz Arduino
#endif

#if defined (ECHO_PIN) && defined (TRIG_PIN)
    #define DIST_SENSOR_USED
    #define TOPIC_DIST "dist"
    float v = 331.5+0.6*20;     // m/s
#elif defined (ECHO_PIN) || defined (TRIG_PIN)
    #error "ECHO_PIN and TRIG_PIN should be defined together. Only one specify"
#endif

#ifdef LIGHT_SENSOR_PIN
    #define TOPIC_LIGHT "lum"
    #ifdef ARDUINO_ARCH_ESP32
        #define LIGHT_LED_PIN 26
    #endif
    int photocellReading;       // the analog reading from the analog resistor divider
#endif

#ifdef MOTION_PIN
    #define TOPIC_MOTION "mouv"
    int pirState = LOW;         // we start, assuming no motion detected
    int detect = 0;             // variable for reading the pin status
#endif

void logValue(const char category[], float value) {
#ifdef WIFI_USED
    String raw{value};
    String topic{TOPIC_PREFIX TOPIC_MCU} ;
    topic += category;
    client.publish(topic.c_str(), raw.c_str());
#endif
}

#ifdef WIFI_USED
    void setup_wifi() {
        delay(10);
        // We start by connecting to a WPA/WPA2 network
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(WIFI_SSID);

        // Set WiFi to station mode and disconnect from an AP if it was previously connected
        #ifdef ARDUINO_ARCH_ESP8266
            WiFi.mode(WIFI_STA);
        #endif

        #ifdef ARDUINO_ARCH_ESP32
            WiFi.mode(WIFI_STA);
        #endif

        WiFi.begin(WIFI_SSID, WIFI_PWD);

        while (WiFi.status() != WL_CONNECTED) {
            Serial.print("Attempting to connect to WPA SSID: ");
            Serial.println(WIFI_SSID);
            // wait 5 seconds for connection:
            Serial.print("Status = ");
            Serial.println(WiFi.status());
            delay(500);
        }
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());

        #ifdef GEOLOCATION_USED
            location_t loc = location.getGeoFromWiFi();
            Serial.println("Location request data");
            Serial.println(location.getSurroundingWiFiJson());
            Serial.println("Latitude: " + String(loc.lat, 7));
            logValue(TOPIC_LATITUDE, loc.lat);
            Serial.println("Longitude: " + String(loc.lon, 7));
            logValue(TOPIC_LONGITUDE, loc.lon);
            Serial.println("Accuracy: " + String(loc.accuracy));
        #endif
    }

    void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    }

    //Reconnect
    void reconnect() {
        // Loop until we're reconnected
        while (!client.connected()) {
            Serial.print("Attempting MQTT connection...");
            if (client.connect(ESP_TYPE "Client")) {
                Serial.println("connected");
            }
            else {
                Serial.print("failed with state, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
                // Wait before retrying
                delay(100);
            }
        }
    }
#endif    

void setup() { // to run once
    Serial.begin(115200);         // Initialize the serial port

    #ifdef DHT_PIN
        dht.begin();
    #endif
    #ifdef DIST_SENSOR_USED
        pinMode(TRIG_PIN, OUTPUT);
        digitalWrite(TRIG_PIN, LOW);
        pinMode(ECHO_PIN, INPUT);
    #endif
    #ifdef LIGHT_PIN
        pinMode(LIGHT_LED_PIN, OUTPUT);     // declare led pin as OUTPUT
    #endif
    #ifdef MOTION_PIN
        pinMode(MOTION_PIN, INPUT);  // declare sensor as INPUT
        // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
    #endif

#ifdef WIFI_USED
    setup_wifi();
    client.setServer(MQTT_IP, MQTT_PORT);
    client.setCallback(callback);
#endif
}

void loop() {
#ifdef WIFI_USED
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
#endif

    //Publish The Sensor Data
    #ifdef DHT_PIN
    {
        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t)) {
            if(DHT_TYPE == DHT22)
                Serial.println("Failed to read from DHT22 sensor!");
            else if(DHT_TYPE == DHT11)
                Serial.println("Failed to read from DHT11 sensor!");
        } else {
            Serial.print(h);
            logValue(TOPIC_HUM, h);
            Serial.print(" \t\t");
            Serial.println(t);
            logValue(TOPIC_TEMP, t);
        }
    }
    #endif

    #ifdef DIST_SENSOR_USED
    {
        // send sound pulse
        digitalWrite(TRIG_PIN, HIGH); // pulse started
        delayMicroseconds(12);
        digitalWrite(TRIG_PIN, LOW);  // pulse stopped

        // listen for echo
        float tUs = pulseIn(ECHO_PIN, HIGH); // microseconds
        float distance = tUs / 58;    // cm
        Serial.print(distance, DEC);
        Serial.println("cm");
        logValue(TOPIC_DIST, distance);
    }
    #endif

    #ifdef LIGHT_SENSOR_PIN
    {
        photocellReading = analogRead(LIGHT_SENSOR_PIN); // read the current light levels
        Serial.print("Analog reading = ");
        Serial.print(photocellReading);         // the raw analog reading
        // We'll have a few threshholds, qualitatively determined
        uint8_t pinState;
        if (photocellReading < 10) {
            Serial.println(" - Black");
            pinState =  HIGH;   // turn on light
        } else if (photocellReading < 200) {
            Serial.println(" - Dark");
            pinState = HIGH;    // turn on light
        } else if (photocellReading < 500) {
            Serial.println(" - Light");
            pinState = LOW;    // turn off light
        } else if (photocellReading < 800) {
            Serial.println(" - Luminous");
            pinState = LOW;    // turn off light
        } else {
            Serial.println(" - Bright");
            pinState = LOW;    // turn off light
        }
        #ifdef LIGHT_LED_PIN
        digitalWrite(LIGHT_LED_PIN, pinState);
        #endif
        logValue(TOPIC_LIGHT, photocellReading);
    }
    #endif

    #ifdef MOTION_PIN
    {
        detect = digitalRead(MOTION_PIN);     // read input value
        if (detect == HIGH) {              // check if the input is HIGH
            if (pirState == LOW) {
                // we have just turned on
                Serial.println("Motion detected!");
                // We only want to print on the output change, not state
                pirState = HIGH;
            }
        } else {
            if (pirState == HIGH){
                // we have just turned of
                Serial.println("Motion ended!");
                // We only want to print on the output change, not state
                pirState = LOW;
            }
        }
        logValue(TOPIC_MOTION, detect);
    }
    #endif

    //client.loop();

    // Wait a few seconds between measurements. The DHT22 should not be read at a higher frequency of
    // about once every 2 seconds. So we add a 3 second delay to cover this.
    delay(4000);
}