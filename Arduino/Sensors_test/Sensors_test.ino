/*
   Sensor Node
   Data collection for first checking
   Feb. 20, 2017, 8:24 AM
*/

/*
   Serial: Zigbee
   A0: Flame sensor
   A1: Smoke sensor
*/

//#include "SoftwareSerial.h"

//SoftwareSerial zb(7, 8);

bool BSReady = false; // FALSE

String ZigbeeData = "";
bool ZigbeeDataAvailable = false; // FALSE

const int flamePin = 0;
const int flameLEDPin = 8;
const int smokePin = 1;
const int smokeLEDPin = NULL;
const String flameAlertMsg = "F";
const String smokeAlertMsg = "S";

const float max_sensor_voltage = 5.0;
const float analog_in_resolution = 1023.0;

class Sensor
{
    // Class Member Variables
    // These are initialized at startup
    int sensorPin;      // the number of the LED pin
    int ledPin;
    //unsigned long previousMillis;
    String alertMsg;

  public:
    Sensor(int sPin, int lPin, String alert)
    {
      sensorPin = sPin;
      ledPin = lPin;
      //previousMillis = 0;
      alertMsg = alert;
      
      if(ledPin!=NULL)
      {
        pinMode(ledPin,OUTPUT);
      }
    }

    void check()
    {
      int readValue;
      float voltage;
      String data = "";
      //unsigned long currentMillis = millis();

      //if (currentMillis - previousMillis >= 10)
      //{
      //previousMillis = currentMillis;
      readValue = analogRead(sensorPin);
      //voltage = readValue * (max_sensor_voltage / analog_in_resolution);
      // place if condition here for normal operations

      // sending to base station via Zigbee
      if (readValue >= 50) // 300
      {
        digitalWrite(ledPin,HIGH);
      }
      else
      {
        digitalWrite(ledPin,LOW);
      }
    }
};

Sensor flame(flamePin, flameLEDPin, flameAlertMsg);
Sensor smoke(smokePin, smokeLEDPin, smokeAlertMsg);

void setup() {
  Serial.begin(38400);
  //zb.begin(38400);
}

void loop() {
  flame.check();
  smoke.check();
}
