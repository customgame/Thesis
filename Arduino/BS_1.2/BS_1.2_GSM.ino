/* GSM variables */

//String GSMData = "";  // a string to hold incoming data
String GSMData = "";  // completed string from GSM (to be analyzed)
boolean GSMDataAvailable = false; // whether the string is complete
boolean waitingForResponse = false; // whether a response is being waited for after issuing a command
int GSMOperation = -1; // command or response currently performed or returned by GSM

int executeErrorCommandIndex = -1;
int counter = 0;
boolean GSMReady = false;

class SIM900
{
    int commandIndexNumber = -1; // temp storage for settings index number
    int command[3] = { -1,  // Index 0 - AT+CNMI=2,2,0,0,0
                       -1,  // Index 1 - AT+CMGF=1
                       -1   // Index 2 - AT+CMGS=\"+639233279514\"God bless!
                     }; // 0 - executed ERROR
    // 1 - executed OK
    // -1 - not commanded

  public:
    SIM900(unsigned long baudRate)
    {
      Serial1.begin(baudRate); // GSM serial port
      // setup GSM here
      powerUpGSM();
      
    }

    void setGSMUp()
    {
      if (GSMDataAvailable) // when string from GSM is complete
      {
        //Serial.print("loop: ");
        //Serial.println(GSMData);
        GSMOperation = analyzeGSMData();  // identify data from GSM
        setCommandFlag(GSMOperation); // set commands flags
        GSMData = ""; // clear string
        GSMDataAvailable = false; // clear flag
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
    }

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

    void sendMessage(String msg, String recipient)
    {
      //      String recipient, msg;
      //      switch (type)
      //      {
      //        case 1: recipient = "+639233279514";
      //          msg = "Fire alert!\n";
      //          msg += coords[0];
      //          msg += ",";
      //          msg += coords[1];
      //          break;
      //        case 2: recipient = "+639233279514";
      //          msg = "Smoke alert!\n";
      //          msg += coords[0];
      //          msg += ",";
      //          msg += coords[2];
      //          break;
      //        default: break;
      //      }

      /*
        Serial1.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
        delay(1000);  // Delay of 1000 milli seconds or 1 second
      */

      //      if (type > 0 && type < 3)
      //      {
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
      //}
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

//    boolean isReportConfirmed()
//    {
//      return 
//    }
};

