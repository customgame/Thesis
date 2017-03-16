unsigned long prevMillis=0;
bool zigbeeDataAvailable=false;
String zigbeeData="";

void setup() {
  Serial.begin(57600);
  Serial2.begin(38400);
}

void loop() {
  unsigned long currentMillis;
  
  if(zigbeeDataAvailable)
  {
    Serial.println(zigbeeData);
    zigbeeData="";
    zigbeeDataAvailable=false;
  }

  currentMillis=millis();
  if(currentMillis-prevMillis>999)
  {
    Serial.println("Request\n");
    Serial2.print("#A*");  
    prevMillis=currentMillis;
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
