/*
   GSM and Wireless Sensor-Based System for Fire Notification with Roadway Generation


   Program for Base Station
   Version 1.2

   Astillo, Philip Virgil B.
   Alquizar, Anniely V.
   Montemayor, John Max P.
   Operiano, Sheena Mares Q.
   Pacaldo, Precious P.
   Punay, Ryan Neil C.
*/

/*
   MCU board: Arduino Mega 2560 Rev3
   GSM: SIM900 (geeetech)
   Zigbee: Xbee S2
   GPS: Ublox NEO-6M-0-001
*/

/*
   Serial Connections:

   Serial 1: SIM900
   Serial 2: Xbee S2
   Serial 3: Ublox NEO-6M GPS Module
*/

#include <TinyGPS++.h>

/* General variables */

bool debugMode = false;
bool BSReady = false;

/* GSM variables */

//String GSMData = "";  // a string to hold incoming data
String GSMData = "";  // completed string from GSM (to be analyzed)
boolean GSMDataAvailable = false; // whether the string is complete
boolean waitingForResponse = false; // whether a response is being waited for after issuing a command
int GSMOperation = -1; // command or response currently performed or returned by GSM
int commandIndexNumber = -1; // temp storage for settings index number
int command[3] = { -1,  // Index 0 - AT+CNMI=2,2,0,0,0
                   -1,  // Index 1 - AT+CMGF=1
                   -1   // Index 2 - AT+CMGS=\"+639233279514\"God bless!
                 }; // 0 - executed ERROR
// 1 - executed OK
// -1 - not commanded

int executeErrorCommandIndex = -1;
int counter = 0;
boolean GSMReady = false;

/* Zigbee variables */
bool ZigbeeReady = false;
String ZigbeeData = "";
bool ZigbeeDataAvailable = false;

/* GPS variables */
TinyGPSPlus gps;
float gpsLat, gpsLon;
const int GPSPowerCtrlPin = 44;
unsigned long prev, current, watch, interval = 5000;
bool GPSReady = false;

/* GSM functions */
void powerUpGSM()
{
  pinMode(42, OUTPUT);
  digitalWrite(42, LOW);
  delay(1000);
  digitalWrite(42, HIGH);
  delay(2000);
  digitalWrite(42, LOW);
  delay(3000);

  if (debugMode)
  {
    Serial.println("\npowerUpGSM() done");
  }
}

void powerDownGSM()
{
  Serial1.println("AT+CPOWD=1");

  if (debugMode)
  {
    Serial.println("\npowerDownGSM() done");
  }
}

void redoCommand(int commandIndex)
{
  switch (commandIndex)
  {
    case 0: setToReceiveMessage();
      break;
    case 1: //sendMessage();
      break;
    default:  break;
  }
}

int checkExecuteError() // returns -1 if no execute error, returns command index otherwise
{
  int commandIndex = -1;

  for (int c = 0; c < 3; c++)
  {
    if (command[c] == 0)  // If command error
    {
      commandIndex = c;
    }
  }

  return commandIndex;
}

void setCommandFlag(int GSMOperation)
{
  if (GSMOperation > 9) // If GSMOperation is a command
  {
    commandIndexNumber = GSMOperation - 10;
  }
  else if (GSMOperation > -1)
  {
    command[commandIndexNumber] = GSMOperation;
    waitingForResponse = false;
    commandIndexNumber = -1;
  }
}

int analyzeGSMData()
{
  //Serial.println("getResponse()");
  String data;
  //String response;
  String newMsgTag = "+CMT:";
  String commandString[] = {"AT+CNMI=2,2,0,0,0", "AT+CMGF=1", "AT+CMGS=\"+639233279514\"God bless!"};
  String responseString[] = {"ERROR", "OK"};
  int type = -1; // command or response and which specifically?
  // type < 10 are responses
  // type > 9 are commands

  data = GSMData;

  if (data != NULL)
  {
    if (debugMode)
    {
      Serial.println("data: " + data);
      Serial.println(data.length());
    }

    /*** Analyze the string here ****/
    // Check if a msg is received
    //if(data.length() > 5) Serial.println(data.substring(0,5));
    if (data.length() > 5 && data.substring(0, 5) == newMsgTag)
    {
      // New SMS message is received
      if (debugMode)
      {
        Serial.println("New SMS message is received!");
      }
    }

    if (type < 0)
    {
      for (int c = 0; c < 2; c++)
      {
        if (data == responseString[c])  // If data is a response
        {
          type = c;

          if (debugMode)
          {
            Serial.println(data + " == " + responseString[c]);
            Serial.print("Returning response: ");
            Serial.println(type);
          }

          break;
        }
      }
    }


    if (type < 0) // If not a response
    {
      for (int c = 0; c < 3; c++)
      {
        if (data == commandString[c]) // If data is a command
        {
          type = c + 10;

          if (debugMode)
          {
            Serial.println(data + " == " + commandString[c]);
            Serial.print("Returning command: ");
            Serial.println(type);
          }

          break;
        }
      }
    }

    if (type < 0)
    {
      if (debugMode)
      {
        Serial.print("UNKOWN COMMAND/RESPONSE: ");
        Serial.println(data);
      }
    }
  }

  return type;
}

void sendMessage(int type)
{
  String recipient, msg;
  switch (type)
  {
    case 1: recipient = "+639233279514";
      msg = "Fire alert!\n";
      msg += gpsLat;
      msg += ",";
      msg += gpsLon;
      break;
    case 2: recipient = "+639233279514";
      msg = "Smoke alert!\n";
      msg += gpsLat;
      msg += ",";
      msg += gpsLon;
      break;
    default: break;
  }

  /*
    Serial1.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
  */

  if (type > 0 && type < 3)
  {
    //Serial1.println("AT+CMGS=\"+639233279514\"\r");  // Replace x with mobile number
    Serial1.println("AT+CMGS=\"" + recipient + "\"\r");
    delay(1000);
    //Serial1.println("God bless!"); // The SMS text you want to send
    //Serial1.println(gpsLat);
    //Serial1.println(gpsLon);
    Serial1.println(msg);
    delay(100);
    Serial1.println((char)26); // ASCII code of CTRL+Z
    delay(1000);
    //Serial1.print("AT+CMGS=\"+639233279514\"God bless!");
    //Serial1.println((char)26);
    waitingForResponse = true;
  }
}

void setToTextMode()
{
  Serial1.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
  waitingForResponse = true;
}

void setToReceiveMessage()
{
  //String response;

  //Serial.println("Setting GSM to receive messages: ");
  Serial1.println("AT+CNMI=2,2,0,0,0");  // AT Command to receive a live SMS
  waitingForResponse = true;
}

void GSMSetup()
{
  for (int c = 0; c < 2; c++)
  {
    if (command[c] == -1) // If not commanded
    {
      switch (c)
      {
        case 0: setToReceiveMessage(); // Set to alert upon receiving messages
          break;
        case 1: setToTextMode();
          break;
        default: break;
      }
      break;
    }
    else if (c == 1)  // If all setup commands are executed OK
    {
      GSMReady = true;
      Serial2.print("GSM Setup complete!");
      Serial2.print((char)13);
    }
  }
}

/* Zigbee functions */

int analyzeZigbeeData()
{
  String data;
  String d[] = {"R",  // Module ready
                "F",  // Flame detected
                "S"   // Smoke detected
               };

  int type = -1; // command or response and which specifically?
  // type < 0 means not yet determined
  // type < 10 are responses
  // type > 9 are commands

  data = ZigbeeData;

  if (data != NULL)
  {

    /*** Analyze the string here ****/
    if (type < 0)
    {
      for (int c = 0; c < 3; c++)
      {
        if (data == d[c]) // If data is a command
        {
          type = c;
          break;
        }
      }
    }
  }
  return type;
}

void zPerform(int type)
{
  switch (type)
  {
    case 0: ZigbeeReady = true;
      break;
    case 1:
    case 2: sendMessage(type);
      while (1);
      break;
    default: break;
  }
}

/* GPS functions */

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (Serial3.available())
      gps.encode(Serial3.read());
  } while (millis() - start < ms);
}

static void gpsCheck()
{
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    //Serial.println(F("No GPS data received: check wiring"));
    Serial2.print("No GPS data received: check wiring");
    Serial2.print((char)13);
  }

  gpsLat = gps.location.lat();
  gpsLon = gps.location.lng();

  smartDelay(500);

}

void powerUpGPS()
{
  digitalWrite(GPSPowerCtrlPin, HIGH);
}

void powerDownGPS()
{
  digitalWrite(GPSPowerCtrlPin, LOW);
}

void setup() {
  if (debugMode)
  {
    Serial.begin(19200);
  }

  Serial1.begin(19200); // GSM serial port
  Serial2.begin(38400); // ZigBee serial port
  Serial3.begin(9600);  // GPS serial port

  pinMode(GPSPowerCtrlPin, OUTPUT); // set GPS power control pin mode to OUTPUT

  powerUpGSM(); // Turn SIM900 on
  powerUpGPS();
  delay(1000);
}

void loop() {
  if (GSMDataAvailable) // when string from GSM is complete
  {
    //Serial.print("loop: ");
    //Serial.println(GSMData);
    GSMOperation = analyzeGSMData();  // identify data from GSM
    setCommandFlag(GSMOperation); // set commands flags
    GSMData = ""; // clear string
    GSMDataAvailable = false; // clear flag
  }

  if (ZigbeeDataAvailable)
  {
    // analyze Zigbee data here
    Serial2.print("You replied \"");
    Serial2.print(ZigbeeData);
    Serial2.print("\"");
    Serial2.print((char)13);
    zPerform(analyzeZigbeeData());
    ZigbeeData = "";
    ZigbeeDataAvailable = false;
    //while(1);
  }

  if (!waitingForResponse)
  {
    if (!GSMReady)
    {
      if (debugMode)
      {
        Serial.println("Checking GSM setup...");
      }
      GSMSetup(); // perform GSM necessary setup
    }
    else
    {
      executeErrorCommandIndex = checkExecuteError(); // check for command execution error
      if (executeErrorCommandIndex > -1) // if a command index is returned
      {
        if (debugMode)
        {
          Serial.print("Command error: ");
          Serial.println(executeErrorCommandIndex);
        }

        redoCommand(executeErrorCommandIndex);  // redo command
        //while (1);
      }
    }
  }//else count millis until declare GSM as unresponsive

  /* GPS */
  if (!GPSReady)
  {
    if (!gps.location.isValid() || !gps.location.isValid())
    {
      gpsCheck();
      //Serial2.println("gpsCheck()");
    }
    else
    {
      gpsCheck(); // get lat and long
      powerDownGPS();
      GPSReady = true;
      //sendMessage();
      //Serial2.println("N");
      Serial2.print(gpsLat);
      Serial2.print(",");
      Serial2.print(gpsLon);
      Serial2.print((char)13);
    }
  }

  if (!ZigbeeReady)
  {
    Serial2.print("T"); // Test/ping sensor nodes
    Serial2.print((char)13);
  }

  if (GSMReady && GPSReady && ZigbeeReady && !BSReady)
  {
    BSReady = true;
    Serial2.print("N"); // Normal operations
    Serial2.print((char)13);
  }
}

void serialEvent1()
{
  char in;
  if (Serial1.available())
  {
    in = Serial1.read();
    //Serial.println(Serial1.read());
    if (in == 13);
    else if (in == 10)
    {
      GSMDataAvailable = true;
    }
    else
    {
      GSMData += in;
    }
  }
}

void serialEvent2()
{
  char in;
  if (Serial2.available() > 0)
  {
    in = (char)Serial2.read();

    if (in == 13)
    {
      ZigbeeDataAvailable = true;
    }
    else
    {
      ZigbeeData += in;
    }
  }
}
