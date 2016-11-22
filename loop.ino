void loop() {
  int currState = digitalRead(button);
  bool changeState = ( currState == laststate);
  if(changeState){
    laststate =  currState;
    if(currState == LOW){
      if(leaderAddress == myAddress){
          sendCommand(fullHEX, (uint8_t*)&cureHEX, 1);
          isInfected = false;
        }
        else isInfected = true; 
    }
  }
  laststate = currState;

  lightLED();
  readAndHandlePackets();
  if (isElecting) {
    isElecting = false;
    LeaderNoResponse = millis() + LeaderBroadcastPeriod;
    if (getALeader) electNewLeader();
    else {
      sendCommand(fullHEX, (uint8_t*)&battleLeaderHEX, 1);
      leaderAddress = myAddress;
    }
  }
   else {
    if (millis() > LeaderNoResponse) {
      if (leaderAddress == myAddress) {
        sendCommand(fullHEX, (uint8_t*) &leaderAliveHEX, 1);
        LeaderNoResponse = millis() + LeaderBroadcastPeriod / 3;
      } else {
        Serial.println("Original leader dead, relecting new leader.");
        electNewLeader();
      }
    }
    if (leaderAddress != myAddress && isInfected) {
      sendCommand(fullHEX, (uint8_t*) &infectHEX, 1);
    }
  }
}
