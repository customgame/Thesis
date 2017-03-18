unsigned long prevMillis = 0;
bool zigbeeDataAvailable = false;
String zigbeeData = "";
char snId[3] = {'A', 'B', 'C'};
int snSwitch = -1;
int prevStatusF[3] = {0, 0, 0},
                     prevStatusS[3] = {0, 0, 0},
                                      alert = false;
String msg = "";
boolean GSMDataAvailable = false; // whether the string is complete
String GSMData = "";  // completed string from GSM (to be analyzed)

void setToTextMode()
{
  Serial1.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
  //waitingForResponse = true;
}

void sendMessage(String m, String recipient)
{
  //Serial1.println("AT+CMGS=\"+639233279514\"\r");  // Replace x with mobile number
  Serial1.println("AT+CMGS=\"" + recipient + "\"\r");
  delay(1000);
  //Serial1.println("God bless!"); // The SMS text you want to send
  //Serial1.println(gpsLat);
  //Serial1.println(gpsLon);
  Serial1.println(m);
  delay(100);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  //Serial1.print("AT+CMGS=\"+639233279514\"God bless!");
  //Serial1.println((char)26);
  //waitingForResponse = true;
  //}
  //Serial.println("Send message");
}

bool idMatches(String data, char id)  // Function which compares data with Sensor node ID
{
  bool idMatches; // Flag for ID matching

  if (data[1] == id) idMatches = true;  // if data matches ID of this node, set idMatches flag to TRUE
  else idMatches = false; // else, set it to FALSE

  return idMatches; // Return flag
}

bool statusChanged(String data)  // Function which determines any change in sensor status
{
  bool statusChanged; // status changed flag
  String currentStatusFString = "",
         currentStatusSString = "";
  currentStatusFString += data[2];
  currentStatusSString += data[3];
  int currentStatusF = currentStatusFString.toInt(),
      currentStatusS = currentStatusSString.toInt();

  if ((prevStatusF[snSwitch] ^ currentStatusF) || (prevStatusS[snSwitch] ^ currentStatusS)) // If there is any change in statuses
  {
    prevStatusF[snSwitch] = currentStatusF; // Store new Flame sensor status
    prevStatusS[snSwitch] = currentStatusS; // Stor new Smoke sensor status
    statusChanged = true; // Set statusChanged flag to TRUE
  }
  else
  {
    statusChanged = false;  // Set statusChanged flag to FALSE
  }

  return statusChanged; // Return statusChanged flag
}

void setup() {
  Serial.begin(57600);
  Serial1.begin(19200);
  Serial2.begin(38400);
  setToTextMode();
}

void loop() {
  unsigned long currentMillis;
  String packet = "";

  currentMillis = millis();
  if (currentMillis - prevMillis > 999)
  {
    prevMillis = currentMillis;

    switch (snSwitch)
    {
      case 0:
      case 1: snSwitch++;
        break;
      case -1:
      case 2:
      default: snSwitch = 0;
        break;
    }

    packet += "#";
    packet += snId[snSwitch];
    packet += "*";
    Serial.print("Sending request to ");
    Serial.println(snId[snSwitch]);
    Serial2.print(packet);
  }

  if (zigbeeDataAvailable)
  {
    if (idMatches(zigbeeData, snId[snSwitch]))
    {
      if (statusChanged(zigbeeData))
      {
        msg += 10.302163;
        msg += ';';
        msg += 123.954702;
        msg += ';';
        msg += zigbeeData[2];
        msg += ';';
        msg += zigbeeData[3]; //lat,lon,smoke,flame
        sendMessage(msg, "09236213090"); //09052851879 //09332340384
        Serial.println(zigbeeData);
        msg = "";
      }
    }
    zigbeeData = "";
    zigbeeDataAvailable = false;
  }

  if (GSMDataAvailable) // when string from GSM is complete
  {
    Serial.println(GSMData);
    GSMData = "";
    GSMDataAvailable = false;
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

//void serialEvent1()
//{
//  char in;
//  while (Serial1.available())
//  {
//    in = Serial1.read();
//      GSMData += in;
//  }
//  GSMDataAvailable = true;
//}

void serialEvent2()
{
  char in;
  if (Serial2.available() > 0)
  {
    in = (char)Serial2.read();
    zigbeeData += in;
    if ((in == '*') && (zigbeeData.length() == 5))
    {
      zigbeeDataAvailable = true;
    }
    else if ((in == '*') && (zigbeeData.length() != 5))
    {
      zigbeeData = "";
    }
  }
}
