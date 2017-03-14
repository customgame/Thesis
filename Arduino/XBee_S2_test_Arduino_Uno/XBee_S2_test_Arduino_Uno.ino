const String nodeName="SN1";  // CHANGE THIS

void setup() {
  Serial.begin(38400);
}

void loop() {
  Serial.print(nodeName);
  Serial.print((char)13);
  delay(1000);
}
