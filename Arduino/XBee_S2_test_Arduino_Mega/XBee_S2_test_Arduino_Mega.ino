String zigbeeData="";
bool zigbeeDataAvailable=false;
void setup() {
  Serial.begin(9600);
  Serial2.begin(38400);
}

void loop() {
  if(zigbeeDataAvailable)
  {
    Serial.print(zigbeeData);
    zigbeeData="";
    zigbeeDataAvailable=false;
  }
}

void serialEvent2()
{
  char in;
  if (Serial2.available() > 0 && !zigbeeDataAvailable)
  {
    in = (char)Serial2.read();
    
    if (in == 13)
    {
      zigbeeData += "\n";
      zigbeeDataAvailable = true;
    }
    else
    {
      zigbeeData += in;
    }
  }
}
