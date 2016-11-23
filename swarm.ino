#include <XBee.h>
#include <SoftwareSerial.h>

// Declare port
int red = 5,
    green = 6,
    blue = 7,
    button = 8;

XBee xbee = XBee();
SoftwareSerial xbeeSerial(2, 3);
ZBRxResponse rxResponse = ZBRxResponse();
ZBTxRequest txRequest;
//AtCommandRequest atRequest = AtCommandRequest((uint8_t*){'S', 'L'});
AtCommandRequest atRequest = AtCommandRequest("SL");
AtCommandResponse ATResponse;
// Declare time variables
int LeaderBroadcastPeriod = 6000,
    immunityTime = 3000;
uint32_t LeaderNoResponse,
         immunityTimeout;
uint8_t heartbeatsLost = 0;

// Declare all the hex code within the frame
const uint8_t electHEX = 0xB0,
              doneElectionHEX = 0xB1,
              battleLeaderHEX = 0xB2,
              infectHEX = 0xB3,
              cureHEX = 0xB4,
              dicoverHEX = 0xB5,
              leaderAliveHEX = 0xB6,
              fullHEX = fullHEX;
              

uint32_t myAddress, 
         leaderAddress, 
         remoteAddress, 
         addressList[10];
int numDevices = 0;
bool isInfected = false,
     isElecting = false, 
     getALeader = false;
int state = LOW, laststate = HIGH;

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

void lightLED() {
  if (myAddress == leaderAddress) {
    digitalWrite(blue, HIGH);
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
  } 
  else {
    digitalWrite(blue, LOW);
    if (isInfected) {
      digitalWrite(green, LOW);
      digitalWrite(red, HIGH);
    } 
    else {
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
    }
  }
}

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

void electNewLeader(void) {
  if (isElecting)  return;
  isElecting = true;
  Serial.println("Start to elect a new leader");
  getALeader = false;
  uint8_t countDevices = 0;
  for (int i = 0; i < numDevices; i++) {
    if (addressList[i] > myAddress) {
      sendCommand(addressList[i], (uint8_t*) &electHEX, 1);
      countDevices++;
    }
  }
}

void readAndHandlePackets(void) {
  if (xbee.readPacket(1) && xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
    xbee.getResponse().getZBRxResponse(rxResponse);
    remoteAddress = rxResponse.getRemoteAddress64().getLsb();
    bool inList = false;
    for (int i = 0; i < numDevices; i++)
      if (addressList[i] == remoteAddress)
        inList = true;
    if (!inList) addressList[numDevices++] = remoteAddress;
    switch (rxResponse.getData(0)) {
      case dicoverHEX:
        if (rxResponse.getDataLength() > 1) {
          memcpy(&leaderAddress, rxResponse.getData() + 1, sizeof(leaderAddress));
          if (leaderAddress < myAddress) electNewLeader();
        } else {
          uint8_t msgPayload[5];
          msgPayload[0] = dicoverHEX;
          memcpy(msgPayload + 4, &leaderAddress, sizeof(leaderAddress));
          sendCommand(remoteAddress, msgPayload, 5);
        }
        break;

      case electHEX:
        sendCommand(remoteAddress, (uint8_t*)&doneElectionHEX, 1);
        electNewLeader();
        break;

      case doneElectionHEX:
        getALeader = true;
        break;

      case battleLeaderHEX:
        if (remoteAddress > myAddress) {
          leaderAddress = remoteAddress;
          isElecting = false;
          LeaderNoResponse = millis() + LeaderBroadcastPeriod;
        }
        else electNewLeader();
        break;

      case leaderAliveHEX:
        if (myAddress == leaderAddress) {
          if (remoteAddress > myAddress){
            leaderAddress = remoteAddress;
            LeaderNoResponse = millis() + LeaderBroadcastPeriod;
          }
        } else LeaderNoResponse = millis() + LeaderBroadcastPeriod;
        break;

      case infectHEX:
        if (leaderAddress != myAddress)
          isInfected = true;
        break;

      case cureHEX:
        isInfected = false;
        immunityTimeout = millis() + immunityTime;
        break;
    }
  }
}
