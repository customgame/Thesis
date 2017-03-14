const int GPSPowerCtrlPin = 44;
const unsigned long GSMBaudRate = 19200,
                    xbeeBaudRate = 38400,
                    GPSBaudRate = 9600;
const unsigned long GPSTimeout = 5000;
boolean flame[3] = {false, false, false}, smoke[3] = {false, false, false}, alert = false;

SIM900 gsm(GSMBaudRate);
GPS gps(GPSBaudRate, GPSPowerCtrlPin, GPSTimeout);
Zigbee xbee(xbeeBaudRate);
String a = "aw", b = "awews";
String data;
String recipient = "09233279514", msg = "";
float *coords = NULL;
unsigned long prevMillis;
const unsigned long HQConfirmationTimeout = 10000;

void setup() {

  if (debugMode)
  {
    Serial.println("start");
  }
}

void loop() {
  gsm.setGSMUp();

  data = xbee.listn();

  if (data != NULL)
  {
    switch (data[1])  // Flame?
    {
      case 82: if (BSReady)
        {
          Serial2.print("N"); // Normal operations
          Serial2.print((char)13);
        }
        break;
      case 0: if (flame)
        {
          flame[data[0]] = false;
          alert = true;
        }
        break;
      case 1: if (!flame)
        {
          flame[data[0]] = true;
          alert = true;
        }
        break;
      default: break;
    }

    switch (data[2])
    {
      case 0: if (smoke)
        {
          smoke[data[0]] = false;
          alert = true;
        }
        break;
      case 1: if (!smoke)
        {
          smoke[data[0]] = true;
          alert = true;
        }
      default: break;
    }

    if (alert)
    {
      if (coords == NULL)
      {
        coords = gps.getCoords();
      }

      msg += coords[0];
      msg += coords[1];
      msg += flame[0] || flame[1] || flame[2];
      msg += smoke[0] || smoke[1] || smoke[2];

      do
      {
        gsm.sendMessage(msg, recipient);
        prevMillis = millis();
        while (millis() - prevMillis < HQConfirmationTimeout)
        {
          if(gsm.isReportConfirmed)
          {
            break;
          }
        }
      } while (!gsm.isReportConfirmed);

      alert = false;
    }
  }

  if (GSMReady && GPSReady && !BSReady)
  {
    BSReady = true;
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

