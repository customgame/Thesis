#include <TinyGPS++.h>

/* GPS variables */
bool GPSReady = false;

class GPS
{
    TinyGPSPlus tgpsp;
    int GPSPowerCtrlPin;
    unsigned long timeout;
    float gpsLat, gpsLon;
    float coords[2];
    unsigned long prev, current, watch;

  public:
    GPS(unsigned long baudRate, int pin, unsigned long to)
    {
      timeout = to;
      GPSPowerCtrlPin = pin;

      Serial3.begin(baudRate);  // GPS serial port
      powerUpGPS();
    }

    void check()
    {
      if (!GPSReady)
      {
        if (!tgpsp.location.isValid() || !tgpsp.location.isValid())
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
          //          Serial2.print(gpsLat);
          //          Serial2.print(",");
          //          Serial2.print(gpsLon);
          //          Serial2.print((char)13);
        }
      }
    }

    void smartDelay(unsigned long ms) // static
    {
      unsigned long start = millis();
      do
      {
        while (Serial3.available())
          tgpsp.encode(Serial3.read());
      } while (millis() - start < ms);
    }

    void gpsCheck() // static
    {
      if (millis() > timeout && tgpsp.charsProcessed() < 10)
      {
        //Serial.println(F("No GPS data received: check wiring"));
        if (debugMode)
        {
          Serial.print("No GPS data received: check wiring"); // serial2
          Serial.print((char)13); // serial2
        }

      }

      gpsLat = tgpsp.location.lat();
      gpsLon = tgpsp.location.lng();

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

    float* getCoords()
    {
      coords[0] = gpsLat;
      coords[1] = gpsLon;

      return coords;
    }
};

