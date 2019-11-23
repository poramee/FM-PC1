#include "DataProtocol_PC1.hpp"

//PC1
int PC_1::sendFrameCount = 0;
int PC_1::receiveFrameCount = 0;


void PC_1::startSend(int command) {
  ACKStatus status;
  Serial.print("Start Sending Session [");
  Serial.print(command);
  Serial.println("]");
  do {
    // Serial.println("Making Frame...");
    int bits = PC_1::makeFrame(command, sendFrameCount);
    for (int i = 7; i >= 0; --i) {
      Serial.print((bits >> i) & 1);
    }
    Serial.println();
    // Serial.println("Sending...");
    sendFrameDAC(bits, 8);
    // Serial.println("Sent! Waiting for ACK");
    status = PC_1::waitingForACK();
  } while (status != ACKStatus::R);
  PC_1::sendFrameCount = (sendFrameCount + 1) % 2;
  Serial.println("Sending Session Completed");
}

long PC_1::startReceive() {
  long receive = 0;
  bool isReceived = false;
  Serial.println("Waiting for Frame...");
  while (!isReceived) {
    isReceived = receiveFrameDAC(&receive, 16, 500);
    bool crc = checkCRC(receive,16);
    if (isReceived && ((receive >> 13) & 1) == receiveFrameCount &&
        ((receive >> 14) & 3) != 0) {
      if(crc){
        receiveFrameCount = (receiveFrameCount + 1) % 2;
        Serial.println("Data Received, Sending ACK");
        while(isReceived){
          PC_1::sendACK();
          long tmp = 0;
          isReceived = receiveFrameDAC(&tmp, 16, 900);
          if (tmp != 0 && tmp == receive) isReceived = true;
        }
        return receive;
      }
      else{
        receive = 0;
        Serial.println("Incorrect Data, Frame Discarded");
        isReceived = false;
      }
    } else isReceived = false;
  }
  
  return receive;
}
void PC_1::sendACK() {
  int bits = PC_1::makeFrame(0, receiveFrameCount);
  sendFrameDAC(bits, 8);
}
int PC_1::makeFrame(int command, int frameNo) {
  /*
    [3] command + [1] frameNo + [4] CRC
  */
  int bits = 0;
  bits = command;
  bits <<= 1;
  bits += frameNo;
  bits <<= 4;
  bits += generateCRC(bits, 8);
  return bits;
}

ACKStatus PC_1::waitingForACK() {
  long receive = 0;
  bool isReceived = receiveFrameDAC(&receive, 16,500);
  if (isReceived && ((receive >> 14) & 11) == 0 &&
      ((receive >> 13) & 1) == (PC_1::sendFrameCount + 1) % 2) {
    if (checkCRC(receive, 16)){
      Serial.println("ACK Received");
      return ACKStatus::R;
    }
    else{
      Serial.println("Frame Received, Incorrect Data");
      return ACKStatus::NR;
    }
  }
  Serial.println("ACK Not Received");
  return ACKStatus::NR;
}