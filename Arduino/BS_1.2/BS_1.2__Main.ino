const int GPSPowerCtrlPin = 44;
const unsigned long GSMBaudRate = 19200,
                    xbeeBaudRate = 38400,
                    GPSBaudRate = 9600;
const unsigned long GPSTimeout = 5000;
int prevStatusF[3] = {0, 0, 0},
                     prevStatusS[3] = {0, 0, 0},
                                      alert = false;

char snId[3] = {'A', 'B', 'C'};
int snSwitch = -1;

SIM900 gsm(GSMBaudRate);
GPS gps(GPSBaudRate, GPSPowerCtrlPin, GPSTimeout);
Zigbee xbee(xbeeBaudRate);

String a = "aw", b = "awews";
String data;
String recipient = "09233279514", msg = "";
float *coords = NULL;
unsigned long prevMillisSwitch = 0,
              prevMillisWaitHQResponse = 0;
const unsigned long HQConfirmationTimeout = 10000;

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
  if (debugMode)
  {
    Serial.println("start");
  }
}

void loop() {
  unsigned long currentMillis;
  String packet = "";

  gsm.setGSMUp();

  currentMillis = millis();
  if (currentMillis - prevMillisSwitch > 999)
  {
    prevMillisSwitch = currentMillis;

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

    Serial.println("Request\n");
    packet += "#" + snId[snSwitch];
    packet += "*";
    Serial2.print(packet);
  }

  data = xbee.listn();

  if (data != "")
  {
    if (idMatches(data, snId[snSwitch]))
    {
      if (statusChanged(data))
      {
        if (coords == NULL)
        {
          coords = gps.getCoords();
        }

        msg += coords[0];
        msg += coords[1];
        msg += prevStatusF[0] || prevStatusF[1] || prevStatusF[2];
        msg += prevStatusS[0] || prevStatusS[1] || prevStatusS[2];

        do
        {
          gsm.sendMessage(msg, recipient);  // Send an SMS message to the Headquarters
          prevMillisWaitHQResponse = millis();
          while (millis() - prevMillisWaitHQResponse < HQConfirmationTimeout)
          {
            if (/*gsm.isReportConfirmed*/1) // wala pani nga function
            {
              break;
            }
          }
        } while (/*!gsm.isReportConfirmed*/1);
      }
    }
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
