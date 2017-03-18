/* Zigbee variables */

bool ZigbeeReady = false;
String zigbeeData = "";
bool zigbeeDataAvailable = false;

class Zigbee
{
  public:
    Zigbee(long baudRate)
    {
      Serial.begin(baudRate);
      Serial.println("Zigbee constructor");
    }

    String listn()
    {
      String data = "";

      if (zigbeeDataAvailable)
      {
        data = zigbeeData;
        zigbeeData = "";
        zigbeeDataAvailable = false;
      }

      return data;
    }

    void pingSN()
    {
      if (!ZigbeeReady)
      {
        Serial2.print("T"); // Test/ping sensor nodes
        Serial2.print((char)13);
      }
    }

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

      data = zigbeeData;

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

    //    void zPerform(int type)
    //    {
    //      switch (type)
    //      {
    //        case 0: ZigbeeReady = true;
    //          break;
    //        case 1:
    //        case 2: //sendMessage(type);
    //          while (1);
    //          break;
    //        default: break;
    //      }
    //    }
};

