void setup() {
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.begin(xbeeSerial);
  delay(1000);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);

  digitalWrite(blue, HIGH);
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  getAddress();
  leaderAddress = myAddress;
  sendCommand(fullHEX, (uint8_t*)&dicoverHEX, 1);
  LeaderNoResponse = millis() + LeaderBroadcastPeriod;
}
