/********************
 * Program:  DHT22 humidity-temperature sensor tester, 
 *           HC-SR04 sensor tester
 *           Light sensor Tester
 *           PIR sensor tester
 * Description: print humidity temperature to serial 
 *              print distance to serial
 *              print motion movement
 *              print light boolean
 ********************/

#include <DHT.h>

//Constants

//#define DHT_SENSOR
//#define DISTANCE_SENSOR
#define LIGHT_SENSOR
//#define MOTION_SENSOR

#ifdef DHT_SENSOR
  #define DHT_PIN 0           // what pin on the arduino is the DHT22 data line connected to
  #define DHT_TYPE DHT22      // DHT 22  (AM2302)
  //#define DHT_TYPE DHT11    // DHT 11
  #include <DHT.h>
  DHT dht(DHT_PIN, DHT_TYPE); // Initialize DHT sensor for normal 16mhz Arduino
#endif

#ifdef DISTANCE_SENSOR
  #define ECHO_PIN 2
  #define TRIG_PIN 4
  float v=331.5+0.6*20;        // m/s
  // Define a new function that reads and convert the raw reading to distance
  float distance_centimetre() {
    // send sound pulse
    digitalWrite(TRIG_PIN, HIGH); // pulse started
    delayMicroseconds(12);
    digitalWrite(TRIG_PIN, LOW);  // pulse stopped
  
    // listen for echo 
    float tUs = pulseIn(ECHO_PIN, HIGH); // microseconds
    float distance = tUs / 58;  // cm
  return distance;
}
#endif

#ifdef LIGHT_SENSOR
  #define LIGHT_PIN 12
  int photocellReading; // the analog reading from the analog resistor divider
#endif

#ifdef MOTION_SENSOR
  #define PIR_PIN 5               // choose the input pin (for PIR sensor)
  int pirState = LOW;             // we start, assuming no motion detected
  int val = 0;                    // variable for reading the pin status
#endif

void setup() { // to run once
  Serial.begin(115200);   // Initialize the serial port

  #ifdef DHT_SENSOR
    Serial.println("DHT22 Humidity - Temperature Sensor");
    Serial.println("RH (%)\t\t Temp (°C)");
    dht.begin();
  #endif DHT_SENSOR
  #ifdef DISTANCE_SENSOR
    pinMode(TRIG_PIN, OUTPUT); 
    digitalWrite(TRIG_PIN, LOW);
    pinMode(ECHO_PIN, INPUT);
  #endif
  #ifdef MOTION_SENSOR
    pinMode(PIR_PIN, INPUT);     // declare sensor as input
  #endif
}

void loop() {

  #ifdef DHT_SENSOR
  {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) && DHT_TYPE==DHT22) {
      Serial.println("Failed to read from DHT22 sensor!");
      return;
    }
    else if (isnan(h) || isnan(t) && DHT_TYPE==DHT11) {
      Serial.println("Failed to read from DHT11 sensor!");
      return;
    }
    Serial.print(h); 
    Serial.print(" \t\t");
    Serial.println(t);
  }
  #endif
  #ifdef DISTANCE_SENSOR
  {
    Serial.print(distance_centimetre(), DEC);
    Serial.println("cm");
  }
  #endif
  #ifdef LIGHT_SENSOR
  {
    photocellReading = analogRead(LIGHT_PIN);
    Serial.print("Analog reading = ");
    Serial.print(photocellReading); // the raw analog reading
    // We'll have a few threshholds, qualitatively determined
    if (photocellReading < 10) {
      Serial.println(" - Black");
    } else if (photocellReading < 200) {
      Serial.println(" - Dark");
    } else if (photocellReading < 500) {
      Serial.println(" - Light");
    } else if (photocellReading < 800) {
      Serial.println(" - Luminous");
    } else {
      Serial.println(" - Bright");
    }
  }
  #endif
  #ifdef MOTION_SENSOR
  {
    val = digitalRead(PIR_PIN);  // read input value
    if (val == HIGH) {            // check if the input is HIGH
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
  }
  #endif

  // Wait a few seconds between measurements. The DHT22 should not be read at a higher frequency of
  // about once every 2 seconds. So we add a 3 second delay to cover this.
  delay(2000);
}
