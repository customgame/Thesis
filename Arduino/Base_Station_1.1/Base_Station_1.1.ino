#include <TinyGPS++.h>

/*
   General variables
   by max
*/

bool BSReady = false;

/*
   GSM variables
   by max
*/

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

/*
   Zigbee variables
   by max
*/

bool ZigbeeReady = false;
String ZigbeeData = "";
bool ZigbeeDataAvailable = false;

/*
   GPS variables
   by Annie
*/

TinyGPSPlus gps;
float gpsLat, gpsLon;
const int GPSPowerCtrlPin = 44;
unsigned long prev, current, watch, interval = 5000;
bool GPSReady = false;

/*
   GSM functions
   by max
*/

void powerUpGSM()
{
  pinMode(42, OUTPUT);
  digitalWrite(42, LOW);
  delay(1000);
  digitalWrite(42, HIGH);
  delay(2000);
  digitalWrite(42, LOW);
  delay(3000);

  Serial.println("\npowerUpGSM() done");
}

void powerDownGSM()
{
  Serial1.println("AT+CPOWD=1");

  Serial.println("\npowerDownGSM() done");
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
    //Serial.println(command[c]);
  }

  return commandIndex;
}

void setCommandFlag(int GSMOperation)
{
  if (GSMOperation > 9) // If GSMOperation is a command
  {
    //Serial.print("GSMOperation: ");
    //Serial.println(GSMOperation);
    commandIndexNumber = GSMOperation - 10;
    //waitingForResponse = true;
    //Serial.print("commandIndexNumber: ");
    //Serial.println(commandIndexNumber);
  }
  else if (GSMOperation > -1)
  {
    //Serial.print("GSMOperation: ");
    //Serial.println(GSMOperation);
    command[commandIndexNumber] = GSMOperation;
    waitingForResponse = false;
    //Serial.print("command[");
    //Serial.print(commandIndexNumber);
    //Serial.print("]: ");
    //Serial.println(command[commandIndexNumber]);
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
    Serial.println("data: " + data);
    Serial.println(data.length());
    /*** Analyze the string here ****/
    // Check if a msg is received
    //if(data.length() > 5) Serial.println(data.substring(0,5));
    if (data.length() > 5 && data.substring(0, 5) == newMsgTag)
    {
      // New SMS message is received
      Serial.println("New SMS message is received!");
    }

    if (type < 0)
    {
      for (int c = 0; c < 2; c++)
      {
        if (data == responseString[c])  // If data is a response
        {
          type = c;
          Serial.println(data + " == " + responseString[c]);
          Serial.print("Returning response: ");
          Serial.println(type);
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
          Serial.println(data + " == " + commandString[c]);
          Serial.print("Returning command: ");
          Serial.println(type);
          break;
        }
      }
    }

    if (type < 0)
    {
      Serial.print("UNKOWN COMMAND/RESPONSE: ");
      Serial.println(data);
    }
  }

  //Serial.print("Returning type: ");
  //Serial.println(type);
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
  /*
    while (response == NULL)
    {
      response = getResponse();
    }
    Serial.println(response);
  */
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
      //powerDownGSM(); // Turn SIM900 off
      //sendMessage();
      //powerDownGSM();
    }
  }
}

/*
   Zigbee functions
   by max
*/

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
    //Serial.println("data: " + data);
    //Serial.println(data.length());

    /*** Analyze the string here ****/
    if (type < 0)
    {
      for (int c = 0; c < 3; c++)
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
    }

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
    case 0: ZigbeeReady = true;
      break;
    case 1:
    case 2: sendMessage(type);
      while(1);
      break;
    default: break;
  }
}

/*
   GPS functions
   by Annie
*/

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
  //delay(10000);
  Serial.begin(19200);
  Serial1.begin(19200); // GSM serial port
  Serial2.begin(38400); // ZigBee serial port
  Serial3.begin(9600);  // GPS serial port
  pinMode(GPSPowerCtrlPin, OUTPUT); // set GPS power control pin mode to OUTPUT

  powerUpGSM(); // Turn SIM900 on
  powerUpGPS();
  delay(1000);
  //Serial1.println("AT+CPBR=?");
  //Serial1.println("AT+CPBR=1,254");
  //Serial.println("TO DO: Continue setup()\n");

  /*
    setToReceiveMessage(); // Set to alert upon receiving messages
    delay(1000);
    Serial.println("done setToReceiveMessage()");
    setToTextMode();
    delay(1000);
    Serial.println("done setToTextMode()");
    sendMessage();
    delay(1000);
    Serial.println("done setToReceiveMessage()");
  */
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
      Serial.println("Checking GSM setup...");
      GSMSetup(); // perform GSM necessary setup
    }
    else
    {
      executeErrorCommandIndex = checkExecuteError(); // check for command execution error
      if (executeErrorCommandIndex > -1) // if a command index is returned
      {
        Serial.print("Command error: ");
        Serial.println(executeErrorCommandIndex);
        redoCommand(executeErrorCommandIndex);  // redo command
        //while (1);
      }
    }
  }//else count millis until declare GSM as unresponsive

  //  if (counter == 1000)
  //  {
  //    //sendMessage();
  //  }
  //  counter++;
  //delay(1000);

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

    //    Serial2.print(gpsLat);
    //    Serial2.print(",");
    //    Serial2.println(gpsLon);
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
/*
  void serialEvent()
  {
  while (Serial.available())
  {
    //Serial.println("serialEvent()");
    // get the new byte:
    char inChar = Serial.read();
    //Serial.println(Serial1.read());
    GSMData += inChar;
    //Serial.println("inChar: "+inChar);
    if (inChar == '\n')
    {
      Serial.println("String complete!");
      GSMDataAvailable = true;
    }
  }
  }
*/

/*
  void serialEvent()
  {
  if(Serial.available()>0)
  {
    Serial1.print(Serial.read());
  }
  }
*/

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

  /*
    char inChar;
    while (Serial1.available())
    {
    //Serial.println("serialEvent1()");
    // get the new byte:
    inChar = Serial1.read();
    //Serial.println(Serial1.read());

    Serial.println(inChar);
    if (inChar == 13)
    {
      Serial.println("Found CR");
      //Serial1.read(); // reading out -1
      //Serial.println(Serial1.read());
      //inChar = Serial1.read();
      if (inChar == '\n')
      {
        Serial.println("Found LF");
        Serial.println("String complete!");
        GSMDataAvailable = true;
      }
    }
    else
    {
      GSMData += inChar;
    }
    }
  */
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
