void readAndHandlePackets(void) {
  if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
    xbee.getResponse().getZBRxResponse(rxResponse);
    remoteAddress64 = rxResponse.getRemoteAddress64().getLsb();
    //    if (remoteAddress64 > leaderAddress64) beginElection();     // VERIFY WHETHER YOU ACTUALLY NEED THIS
    serialLog(true, remoteAddress64, rxResponse.getData(0));

    bool inList = false;
    for (int i = 0; i < numDevices; i++)
      if (listAddress64[i] == remoteAddress64)
        inList = true;
    if (!inList) listAddress64[numDevices++] = remoteAddress64;
    switch (rxResponse.getData(0)) {
      case MSG_DISCOVERY:
        if (rxResponse.getDataLength() > 1) {
          memcpy(&leaderAddress64, rxResponse.getData() + 1, sizeof(leaderAddress64));
          if (leaderAddress64 < myAddress64) beginElection();
        } else {
          uint8_t msgPayload[5];
          msgPayload[0] = MSG_DISCOVERY;
          memcpy(msgPayload + 4, &leaderAddress64, sizeof(leaderAddress64));
          sendCommand(remoteAddress64, msgPayload, 5);
        }
        break;

      case MSG_ELECTION:
        sendCommand(remoteAddress64, (uint8_t*)&MSG_ACK, 1);
        beginElection();
        break;

      case MSG_ACK:
        electionTimeout = millis() + ELECTION_VICTORY_WAIT_PERIOD;
        isAcknowledged = true;
        break;

      case MSG_VICTORY:
        if (remoteAddress64 > myAddress64) {
          leaderAddress64 = remoteAddress64;
          isElecting = false;
          leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
          betweenElectionTimeout = millis() + ELECTION_BETWEEN_WAIT_PERIOD;
        }
        else beginElection();
        break;

      case MSG_HEARTBEAT:
        if (myAddress64 == leaderAddress64) {
          if (remoteAddress64 > myAddress64){
            leaderAddress64 = remoteAddress64;
            leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
          }
        } else leaderHeartbeatTimeout = millis() + LEADER_HEARTBEAT_PERIOD;
        break;

      case MSG_INFECTION:
        if (millis() > immunityTimeout && leaderAddress64 != myAddress64)
          isInfected = true;
        break;

      case MSG_CLEAR:
        isInfected = false;
        immunityTimeout = millis() + IMMUNITY_PERIOD;
        break;
    }
  }
}
