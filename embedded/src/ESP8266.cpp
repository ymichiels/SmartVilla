//-- end test
//#include "data.h"

// define WIFI_SSID, WIFI_PWD
//        MQTT_IP, MQTT_PORT, MQTT_NAME, MQTT_PWD
#include "config_wifi.h"

//#define WIFI_USED
#define SEND_INVALID 1

#define TOPIC_PREFIX "smart-villa/etage1/esp8266/"

#define LIGHT_SENS
#define TOPIC_LIGHT "lum"
#define LIGHT_PIN A0

#define DHT_SENS
#define TOPIC_HUMIDITE "hum"
#define TOPIC_TEMPERATURE "temp"
#define DHT_PIN D4

#define MOTION_SENS
#define TOPIC_MOTION "mouv"
#define PIR_PIN D2

#define DIST_SENS
#define TOPIC_DIST "dist"
#define TRIG_PIN D5
#define ECHO_PIN D6

// CONFIG - DONE

//#define WIFI_USED
#ifdef WIFI_USED
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
WiFiClient client;
PubSubClient psc{MQTT_IP, MQTT_PORT, client};
#endif


#ifdef DHT_SENS
#include <DHT.h>
DHT dht(DHT_PIN, DHT11);
#endif

#ifdef MOTION_SENS
// boolean indicating wether something was detected last cycle
bool motionLastState;
#endif


template<class T>
void logValue(const char category[], const T& value) {
  String raw{value};
#ifdef WIFI_USED
  String topic{TOPIC_PREFIX};
  topic += category;
  psc.publish(topic.c_str(), raw.c_str());
#else
  Serial.print(category);
  Serial.print(" => ");
  Serial.println(raw);
#endif
}


void setup() {
  // initialize serial communication:
  Serial.begin(9600);
#ifdef WIFI_USED
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  
  Serial.print("Connected as ");
  Serial.println(WiFi.localIP());

  // cc "https://iotdesignpro.com/projects/how-to-connect-esp8266-with-mqtt"
  while(!psc.connected()) {
    Serial.println("Connection to MQTT...");
    if (psc.connect("ESP8266Client", MQTT_NAME, MQTT_PWD)) {
      Serial.println("Connected");
    } else {
      Serial.print("failed with state");
      Serial.println(psc.state());
      delay(2000);
    }
  }
#endif
#ifdef LIGHT_SENS
  pinMode(LIGHT_PIN, INPUT);
#endif
#ifdef DHT_SENS
  dht.begin();
#endif
#ifdef MOTION_SENS
  pinMode(PIR_PIN, INPUT);
  motionLastState = false;
#endif
#ifdef DIST_SENS
  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
#endif
}

void loop()
{
  unsigned long start = millis();
  digitalWrite(D0, digitalRead(D0));
#ifdef LIGHT_SENS
  {
    int lum = analogRead(LIGHT_PIN);
    logValue(TOPIC_LIGHT, lum);
  }
#endif

#ifdef DHT_SENS
  {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if(!isnan(temp)){
      logValue(TOPIC_TEMPERATURE, temp);
    } else {
      #if SEND_INVALID
      logValue(TOPIC_TEMPERATURE, "NaN");
      #endif
    }
    if(!isnan(hum)){
      logValue(TOPIC_HUMIDITE, hum);
    } else {
      #if SEND_INVALID
      logValue(TOPIC_HUMIDITE, "NaN");
      #endif
    }
  }
#endif

#ifdef MOTION_SENS
  {
    bool detect = digitalRead(PIR_PIN) == HIGH;
    logValue(TOPIC_MOTION, detect);
  }
#endif

#ifdef DIST_SENS
  {
    float duration, distanceCm;
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN,HIGH);
   
    // convert the time into a distance
    distanceCm = duration / 29.1 / 2 ;
   
    if (distanceCm <= 0){
      #if SEND_INVALID
      logValue(TOPIC_DIST, "Infinity");
      #endif
    } else {
      logValue(TOPIC_DIST, distanceCm);
    }
  }
#endif

  Serial.print("payload computed in ");
  Serial.print(millis() - start);
  Serial.println("ms");
  Serial.println();
  delay(1000);
}