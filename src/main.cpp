#include "DataProtocol_PC1.hpp"
#include <TEA5767Radio.h>
#include <Wire.h>

TEA5767Radio radio = TEA5767Radio();

using namespace PC_1;

void interpret(long command);

bool isScanned = false;
int imgData[3] = {-1,-1,-1};

void printInstruction(){
    Serial.print("[1] Rescan\t");
    for(int i = 2;i <= 4;++i){
      Serial.print("[");
      Serial.print(i);
      Serial.print("] Get Image Data for ");
      if (imgData[i - 2] == 0)
        Serial.print("'Bottom'");
      else if (imgData[i - 2] == 1)
        Serial.print("'Top'");
      else if (imgData[i - 2] == 2)
        Serial.print("'Left'");
      else if (imgData[i - 2] == 3)
        Serial.print("'Right'");
      else if (imgData[i - 2] == 4)
        Serial.print("'Upper'");
      else if (imgData[i - 2] == 5)
        Serial.print("'Lower'");
      Serial.print("\t");
    }
}

void setup() {
  Transceiver::init();
  // Serial.println("<< PC_1 >>");
  Serial.println("Press Any Key to Start Scanning");
  pinMode(13, OUTPUT);
  // radio.setFrequency(107.0);
  radio.setFrequency(90.3);
}

void loop() {
  digitalWrite(13, HIGH);
  long tmp = 0;
  if (Serial.available()) {
    if(!isScanned){
      interpret(1);
      isScanned = true;
      printInstruction();
      Serial.flush();
    }
    else{
      const int read = Serial.read() - '0';
      Serial.println();
      while (Serial.available())
        Serial.read();
      interpret(read);
      printInstruction();
      Serial.flush();
    }
    Serial.flush();
  }
  else if(receiveFrameDAC(&tmp,16,500)){
    sendACK();
    delay(300);
  }


    // Serial.println("SEND");
    // sendFrameDAC(0b00100110, 8);
    // delay(500);

  // long receiveMsg = 0;
  // if (receiveFrameDAC(&receiveMsg, 8, 500)) {
  //   Serial.print("Received Msg: ");
  //   for (int i = 7; i >= 0; --i) {
  //     Serial.print((receiveMsg >> i) & 1);
  //   }
  //   Serial.print("   ");
  //   Serial.println((char)receiveMsg);
  // }
}

void interpret(long command) {
  if (command == 1) {
    // Serial.println(">> Start Scanning Session <<");
    startSend(command);
    long receive = startReceive();

    // Serial.print("Receive Message: ");
    // for (int i = 15; i >= 0; --i) {
    //   Serial.print((receive >> i) & 1);
    // }
    // Serial.println();
    Serial.println();
    Serial.println("[ IMAGE SCANNED ]");
    Serial.print("  < LEFT >  ");
    Serial.print(" < CENTER > ");
    Serial.println("  < RIGHT > ");

    for (int i = 2; i >= 0; --i) {
      const int translate = (receive >> ((i * 3) + 4)) & 7;
      imgData[2 - i] = translate;
      if (translate == 0)
        Serial.print("  'Bottom'  ");
      else if (translate == 1)
        Serial.print("   'Top'    ");
      else if (translate == 2)
        Serial.print("   'Left'   ");
      else if (translate == 3)
        Serial.print("  'Right'   ");
      else if (translate == 4)
        Serial.print("  'Upper'   ");
      else if (translate == 5)
        Serial.print("  'Lower'   ");
    }
    Serial.println();
    Serial.println(">> Scanning Session Completed <<");
    while(Serial.available()) Serial.read();
  } else if (command >= 2 && command <= 4) {
    Serial.println(">> Start Getting Data Session <<");
    startSend(command);
    int imgDots[16][3];
    for (int dot = 0; dot < 16; ++dot) {
      for (int co = 0; co < 3; ++co) {
        Serial.print("[");
        Serial.print(dot);
        Serial.print(", ");
        Serial.print(co);
        Serial.print("] ");
        long receive = startReceive();
        // Serial.print("Received Message: ");
        // for (int i = 15; i >= 0; --i) {
        //   Serial.print((receive >> i) & 1);
        // }
        imgDots[dot][co] = (receive >> 4) & 0b11111111;
        Serial.println();
      }
    }
    Serial.println("[ IMAGE DATA RECEIVED ]");
    for(int i = 0;i < 16;++i){
      Serial.print("Dot #");
      Serial.print(i);
      Serial.print(" @(");
      Serial.print(imgDots[i][0]);
      Serial.print(",");
      Serial.print(imgDots[i][1]);
      Serial.print(") -> ");
      Serial.println(imgDots[i][2]);
    }
    Serial.println(">> Getting Data Session Completed <<");
  } else
      Serial.println("Invalid Command");
}