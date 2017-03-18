#define GSMBAUDRATE 19200
#define XBEEBAUDRATE 38400
#define GSPBAUDRATE 9600
#define GPSPOWERCTRLPIN 44
#define GPSTimeout 5000

SIM900 gsm(GSMBAUDRATE);
GPS gps(GSPBAUDRATE, GPSPOWERCTRLPIN, GPSTimeout);
Zigbee xbee(XBEEBAUDRATE);

int a = 0;

int prevStatusF[3] = {0, 0, 0},
                     prevStatusS[3] = {0, 0, 0},
                                      alert = false;

char snId[3] = {'A', 'B', 'C'};
int snSwitch = -1;

String data;
String recipient = "09321214909", msg = "";
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
  Serial.begin(38400);
  Serial.println("start");
  //gps.check();
}

void loop() {
  unsigned long currentMillis;
  String packet = "";

  //gsm.setGSMUp(); // Set GSM up

  currentMillis = millis(); // Store current lapsed time to currentMillis
  if (currentMillis - prevMillisSwitch > 999) // If lapsed time is 1 second or greater
  {
    prevMillisSwitch = currentMillis; // Store current lapsed time to prevMillisSwitch

    switch (snSwitch)
    {
      case 0:
      case 1: snSwitch++; // If 0 or 1
        break;
      case -1:
      case 2:
      default: snSwitch = 0;  // If -1, 2, etc
        break;
    }

    packet += "#";  // Add header to packet
    packet += snId[snSwitch]; // Add ID to packet
    packet += "*";  // //add trailer to packet
    if (a == 0)
    {
      packet = "";
      packet += 10.302163;
      packet += ",";
      packet += 123.954702;
      Serial.println(packet);

      // Test
      //      do
      //      {
      //        gsm.sendMessage(packet, recipient);  // Send an SMS message to the Headquarters
      //        prevMillisWaitHQResponse = millis();  // Store current lapsed time to prevMillisWaitHQResponse
      //        while (millis() - prevMillisWaitHQResponse < HQConfirmationTimeout) // If HQConfirmationTimtout seconds have passed
      //        {
      //          if (gsm.isReportConfirmed()) // wala pani nga function
      //          {
      //            msg = "";
      //            break;
      //          }
      //        }
      //      } while (!gsm.isReportConfirmed());
    }
    //Serial.println(packet);
    Serial2.print(packet);  // Send packet to Sensor node
  }

  data = xbee.listn();  // Listen for incoming data from zigbee

  if (data != "") // If data is not empty
  {
    if (idMatches(data, snId[snSwitch]))  // If incoming data matches ID
    {
      if (statusChanged(data))  // If household status changed
      {
        if (coords == NULL) // If coords is not filled in
        {
          coords = gps.getCoords(); // Get and store coordinates
        }

        msg += 10.302163; // 10.302163 // coords[0]
        msg += ';';
        msg += 123.954702;  // 123.954702 // coords[1]
        msg += ';';
        msg += data[2];
        msg += ';';
        msg += data[3]; //lat,lon,smoke,flame
        Serial.println(data);
        //gsm.sendMessage(msg, "09236213090"); //09052851879 //09332340384 // 09236213090

        do
        {
          gsm.sendMessage(msg, recipient);  // Send an SMS message to the Headquarters
          prevMillisWaitHQResponse = millis();  // Store current lapsed time to prevMillisWaitHQResponse
          while (millis() - prevMillisWaitHQResponse < HQConfirmationTimeout) // If HQConfirmationTimtout seconds have passed
          {
            if (gsm.isReportConfirmed()) // wala pani nga function
            {
              msg = "";
              break;
            }
          }
        } while (!gsm.isReportConfirmed());
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
