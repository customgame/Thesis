/*
   Sensor Node 2.0
   March 16, 2017, 12:54 AM
*/

/*
   Serial: Zigbee
   A0: Flame sensor
   A1: Smoke sensor
*/

String zigbeeData = ""; // Storage for data from zigbee
bool zigbeeDataAvailable = false; // Flag for zigbee data availability

const int flamePin = 0; // Flame sensor is connected to A0 pin of Arduino Uno
const int smokePin = 1; // Smoke sensor is connected to A1 pin of Arduino Uno

int prevStatusF = 0, // previous Flame sensor status
    prevStatusS = 0; // previous Smoke sensor status

class Sensor
{
    int sensorPin;  // Storage of pin# where the sensor is connected to

  public:
    Sensor(int pin) // Constructor
    {
      sensorPin = pin;  // Store pin to sensorPin
    }

    int check() // Checks current output value of the sensor
    {
      int readValue,  // Storage for Analog In value
          stat; // Sensor status

      readValue = analogRead(sensorPin);  // Read and store current output value of the sensor

      if (readValue >= 300) // If voltage is greater than or equal to 300
      {
        stat = 1; // Set status to HIGH
      }
      else  // If voltage is lesser than 300
      {
        stat = 0; // Set status to LOW
      }

      return stat;  // Return status
    }
};

bool idMatches()  // Function which compares data with Sensor node ID
{
  String data = zigbeeData; // Store zigbee data to new var
  String SNid = "#A*";  // Change letter with regards to Sensor node ID
  bool idMatches; // Flag for ID matching

  if (data != NULL) // If data is not NULL
  {
    if (data == SNid) idMatches = true;  // if data matches ID of this node, set idMatches flag to TRUE
    else idMatches = false; // else, set it to FALSE
  }

  return idMatches; // Return flag
}

bool statusChanged(int currentStatusF, int currentStatusS)  // Function which determines any change in sensor status
{
  bool statusChanged; // status changed flag

  if ((prevStatusF ^ currentStatusF) || (prevStatusS ^ currentStatusS)) // If there is any change in statuses
  {
    prevStatusF = currentStatusF; // Store new Flame sensor status
    prevStatusS = currentStatusS; // Stor new Smoke sensor status
    statusChanged = true; // Set statusChanged flag to TRUE
  }
  else
  {
    statusChanged = false;  // Set statusChanged flag to FALSE
  }

  return statusChanged; // Return statusChanged flag
}

Sensor flame(flamePin);  // Create flame object - Flame sensor
Sensor smoke(smokePin);  // Create smoke object - Smoke sensor

void setup() {
  Serial.begin(38400);  // Set baud rate for Zigbee
}

void loop() {
  int currentStatusF,  // current Flame sensor status
      currentStatusS;  // current Smoke sensor status
  String packet;  // storage for packet to be sent to Base Station

  if (zigbeeDataAvailable)  // If there is data from Base Station
  {
    if (idMatches()) // Check if serial data matches with ID
    {
      currentStatusF = flame.check();  // Check flame status
      currentStatusS = smoke.check();  // Check smoke status
      if (statusChanged(currentStatusF, currentStatusS))  // If there is any change in statuses
      {
        packet = "#A";  // Initialize packet with header and Sensor node ID
        packet += currentStatusF; // Add flame sensor status to packet
        packet += currentStatusF; // Add smoke sensor status to packet
        packet += "*";  // Add trailer to packet
        Serial.print(packet); // Send packet to Base Station
      }
    }
    zigbeeData = "";  // Reset data var
    zigbeeDataAvailable = false;  // Reset data flag
  }
}

void serialEvent()
{
  char in;
  if (Serial.available() > 0)
  {
    in = (char)Serial.read();
    zigbeeData += in;
    if ((in == '*') && (zigbeeData.length() == 3))
    {
      zigbeeDataAvailable = true;
    }
    else if ((in == '*') && (zigbeeData.length() != 3))
    {
      zigbeeData = "";
    }
  }
}
