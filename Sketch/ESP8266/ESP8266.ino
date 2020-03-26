// ====== MOVE IT TO A LIBRARY ======

struct Data {
  String category;
  String reprValue;
};

const int maxItem = 8;
class Payload {
protected:
  int m_current = 0;
  int m_start = 0;
  Data m_data[maxItem];
public:
  void start() {
    m_start = millis();
  }
  void reset() {
    for(Data& elt : *this) {
      elt.category.clear();
      elt.reprValue.clear();
    }
    m_current = 0;
  }
  void add(String category, String repr) {
    std::swap(m_data[m_current].category, category);
    std::swap(m_data[m_current].reprValue, repr);
    if(m_current == maxItem) {
      Serial.println("MAX ITEM REACHED");
    }
    m_current += 1;
  }
  int currentTime() const {
    return millis() - m_start;
  }
  Data* begin(){
    return m_data;
  }
  const Data* begin() const {
    return m_data;
  }
  const Data* cbegin() const {
    return begin();
  }
  Data* end(){
    return m_data + m_current;
  }
  const Data* end() const {
    return m_data + m_current;
  }
  const Data* cend() const {
    return end();
  }
};

// ====== END MOVE IT ====

// from https://github.com/esp8266/Arduino/blob/3e7b4b8e0cf4e1f7ad48104abfc42723b5e4f9be/variants/nodemcu/pins_arduino.h
const uint8_t D0   = 16;
const uint8_t D1   = 5;
const uint8_t D2   = 4;
const uint8_t D3   = 0;
const uint8_t D4   = 2;
const uint8_t D5   = 14;
const uint8_t D6   = 12;
const uint8_t D7   = 13;
const uint8_t D8   = 15;
const uint8_t D9   = 3;
const uint8_t D10  = 1;

//#define WIFI_USED
#define WIFI_SSID "REDACTED"
#define WIFI_PWD "REDACTED"

#define SEND_INVALID 1

#define LIGHT_SENS
#define LIGHT_PIN A0

#define DHT_SENS
#define DHT_PIN D0

#define MOTION_SENS
#define PIR_PIN D2

#define DIST_SENS
#define TRIG_PIN D5
#define ECHO_PIN D6

#ifdef WIFI_USED
#include <ESP8266WiFi.h>
#endif

#ifdef DHT_SENS
#include <DHT.h>
DHT dht(DHT_PIN, DHT11);
#endif

#ifdef MOTION_SENS
// boolean indicating wether something was detected last cycle
bool motionLastState;
#endif


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

Payload payload;

void loop()
{
  payload.start();
  int debut = millis();
#ifdef LIGHT_SENS
  {
    int lum = analogRead(LIGHT_PIN);
    payload.add("light", String(lum));
  }
#endif
#ifdef DHT_SENS
  {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if(!isnan(temp)){
      payload.add("temperature", String(temp));
    } else {
      #if SEND_INVALID
      payload.add("temperature", "NaN");
      #endif
    }
    if(!isnan(hum)){
      payload.add("humidity", String(hum));
    } else {
      #if SEND_INVALID
      payload.add("humidity", "NaN");
      #endif
    }
  }
#endif
#ifdef MOTION_SENS
  {
    bool detect = digitalRead(PIR_PIN) == HIGH;
    if(detect == motionLastState){
      payload.add("pir_changed", "false");
    } else {
      payload.add("pir_changed", "true");
    }

    payload.add("pir_detect ", detect ? "true" : "false");
    motionLastState = detect;
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
      payload.add("distance", "Infinity");
      #endif
    } else {
      payload.add("distance", String(distanceCm));
    }
  }
#endif
  Serial.print("payload computed in ");
  Serial.print(payload.currentTime());
  Serial.println("ms");
  for(Data& item : payload){
    Serial.print(item.category);
    Serial.print(": ");
    Serial.print(item.reprValue);
    Serial.println();
  }
  Serial.println();
  payload.reset();
  delay(1000);
}
