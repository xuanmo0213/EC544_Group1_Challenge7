void electNewLeader() {
  if (isElected)  return;
  isElected = true;
  Serial.println("Start to elect a new leader");
  getToElect = false;
  uint8_t countDevices = 0;
  for (int i = 0; i < numDevices; i++) {
    if (addressList[i] > myAddress) {
      sendCommand(addressList[i], (uint8_t*) &electHEX, 1);
      countDevices++;
    }
  }
}
