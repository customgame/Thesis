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
bool SendData = false;

const int flamePin = 0;
const int smokePin = 1;
const String flameAlertMsg="F";
const String smokeAlertMsg="S";

const float max_sensor_voltage = 5.0;
const float analog_in_resolution = 1023.0;

class Sensor
{
    // Class Member Variables
    // These are initialized at startup
    int sensorPin;      // the number of the LED pin
    //unsigned long previousMillis;
    String alertMsg;

    // Constructor - creates a Flasher
    // and initializes the member variables and state
  public:
    Sensor(int pin, String alert)
    {
      sensorPin = pin;
      //previousMillis = 0;
      alertMsg = alert;
    }

    String check()
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
      if (readValue >= 300) // 300
      {
        return "1";
        //data += sensorPin;
        //data += ",";
        //data += currentMillis;
        //data += ",";
        //data += readValue;
        //data += "\n";
        //Serial.print(alertMsg);
        //Serial.print((char)13);
        //data="";
        //        Serial.print(sensorPin);
        //        Serial.print(",");
        //        Serial.print(currentMillis);
        //        Serial.print(",");
        //        Serial.println(voltage);
      }
      return "0";
      //}
    }
};

int analyzeZigbeeData()
{
  String data;
  //String d[] = {"T",  // Test/ping by base station
  //              "N"   // Normal operations
  //             };

  String d = "#A*";

  int type = -1; // command or response and which specifically?
  // type < 0 means not yet determined
  // type < 10 are responses
  // type > 9 are commands

  data = ZigbeeData;

  if (data != NULL)
  {
    int i = 1;
    if(d == data)i=0;
    
    
    if(i==0)SendData = true;
    else SendData = false;
    
    //Serial.println("data: " + data);
    //Serial.println(data.length());

    /*** Analyze the string here ****/
   /* if (type < 0)
    {
      for (int c = 0; c < 2; c++)
      {
        if (data == d[c]) // If data is a command
        {
          type = c;
          //Serial.println(data + " == " + commandString[c]);
          //Serial.print("Returning command: ");
          //Serial.println(type);
          break;
        }
      }
    }*/

    //    if (type < 0)
    //    {
    //      Serial.print("UNKOWN COMMAND/RESPONSE: ");
    //      Serial.println(data);
    //    }
  }

  //Serial.print("Returning type: ");
  //Serial.println(type);
  return type;
}

void zPerform(int type)
{
  switch (type)
  {
    case 0: Serial.print("R");
      Serial.print((char)13);
      break;
    case 1: BSReady = true;
      break;
    default: break;
  }
}

Sensor flame(flamePin,flameAlertMsg);
Sensor smoke(smokePin,smokeAlertMsg);

void setup() {
  Serial.begin(38400);
  //zb.begin(38400);
}

void loop() {
  String statusF, statusS;
  //  char in;
  //  if (Serial.available() > 0)
  //  {
  //    in = (char)Serial.read();
  //
  //    if (in == 13)
  //    {
  //      ZigbeeDataAvailable = true;
  //    }
  //    else
  //    {
  //      ZigbeeData += in;
  //    }
  //  }

  //if (BSReady)
  //{
   // flame.check();
  //  smoke.check();
    //delay(100);
  //}

  if (ZigbeeDataAvailable)
  {
    // analyze Zigbee data here
    //Serial.println(ZigbeeData);
    //zPerform(analyzeZigbeeData());
    analyzeZigbeeData();
    if(SendData)
    {
      statusF = flame.check();
      statusS = smoke.check();  

      Serial.print("#A"+statusF+statusS+"*");
    }
    ZigbeeData = "";
    ZigbeeDataAvailable = false;
    SendData = false;
  }

  

}

void serialEvent()
{
  char in;
  if (Serial.available() > 0)
  {
    in = (char)Serial.read();

    if (in == '*')
    {
      ZigbeeDataAvailable = true;
    }
    else
    {
      ZigbeeData += in;
    }
  }
  //  String in = "";
  //  while (Serial3.available() > 0)
  //  {
  //    in += (char) Serial3.read();
  //  }
  //  Serial.println(in);
}
