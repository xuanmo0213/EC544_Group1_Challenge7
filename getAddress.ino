void getAddress(void) {
  while(!ATResponse.isOk()){
    while(!xbee.readPacket(5000) || xbee.getResponse().getApiId() != AT_COMMAND_RESPONSE) xbee.send(atRequest);
    xbee.getResponse().getAtCommandResponse(ATResponse);
  }
  for(int i = 0; i < 4; i++) {
    int value = ATResponse.getValue()[i];
    myAddress |= value << 8 * (3 - i);
  }
  Serial.print("myAddress: " + myAddress);
}
