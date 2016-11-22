void sendCommand(uint32_t desAddress, uint8_t* payload, uint8_t length) {
  if (payload[0] == dicoverHEX || payload[0] == battleLeaderHEX) {
    txRequest = ZBTxRequest(XBeeAddress64(0x00000000, fullHEX), payload, length);
    xbee.send(txRequest);
  } else {
    if (desAddress == fullHEX) {
      for (int i = 0; i < numDevices; i++) {
        txRequest = ZBTxRequest(XBeeAddress64(0x0013A200, addressList[i]), payload, length);
        xbee.send(txRequest);
      }
    } else {
      txRequest = ZBTxRequest(XBeeAddress64(0x0013A200, desAddress), payload, length);
      xbee.send(txRequest);
    }
  }
}
