#include "DataProtocol_PC1.hpp"
#include <TEA5767Radio.h>
#include <Wire.h>

TEA5767Radio radio = TEA5767Radio();

using namespace PC_1;

void interpret(long command);


void clrscr(){
      // Clear Screen
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");     // cursor to home command
}


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
  Serial.print("Press Any Key to Start Scanning");
  pinMode(13, OUTPUT);
  radio.setFrequency(105.0);
}

void loop() {
  digitalWrite(13, HIGH);
  if (Serial.available()) {
    if(!isScanned){
      Serial.println(">> Start Scanning Session <<");
      interpret(0);
      isScanned = true;
      Serial.println(">> Scanning Session Completed <<");
      printInstruction();
    }
    else{
      const int read = Serial.read() - '0';
      interpret(read);
    }
  }
  //   sendFrameDAC('K', 8);
  //   long receive = 0;
  //   while (!receiveFrameDAC(&receive, 8, 500)){}
  //   for (int i = 6; i >= 0; i -= 2) {
  //       Serial.print((receive >> (i + 1)) & 1);
  //       Serial.print((receive >> i) & 1);
  //       Serial.print(" ");
  //   }
  //   Serial.println((char)receive);
}

void interpret(long command) {
  if (command == 1) {
    startSend(command);
    long receive = startReceive();

    // Serial.print("Receive Message: ");
    // for (int i = 15; i >= 0; --i) {
    //   Serial.print((receive >> i) & 1);
    // }
    // Serial.println();

    clrscr();

    Serial.println("[ IMAGE SCANNED ]");
    Serial.print("  < LEFT >  ");
    Serial.print(" < CENTER > ");
    Serial.print("  < RIGHT > ");

    // Print illustration of the image
    for (int i = 2; i >= 0; --i) {
      const int translate = (receive >> ((i * 3) + 4)) & 7;
      if (translate == 0){
        Serial.println("    XXXX    ");
        Serial.println("    XXXX    ");
        Serial.println("    OOOO    ");
        Serial.println("    OOOO    ");
      }
      else if (translate == 1){
        Serial.println("    OOOO    ");
        Serial.println("    OOOO    ");
        Serial.println("    XXXX    ");
        Serial.println("    XXXX    ");
      }
      else if (translate == 2){
        Serial.println("    OOXX    ");
        Serial.println("    OOXX    ");
        Serial.println("    OOXX    ");
        Serial.println("    OOXX    ");
      }
      else if (translate == 3){
        Serial.println("    XXOO    ");
        Serial.println("    XXOO    ");
        Serial.println("    XXOO    ");
        Serial.println("    XXOO    ");
      }
      else if (translate == 4){
        Serial.println("    XOOO    ");
        Serial.println("    XXOO    ");
        Serial.println("    XXXO    ");
        Serial.println("    XXXX    ");
      }
      else if (translate == 5){
        Serial.println("    XXXX    ");
        Serial.println("    OXXX    ");
        Serial.println("    OOXX    ");
        Serial.println("    OOOX    ");
      }
    }

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
  } else if (command >= 2 && command <= 4) {
    clrscr();
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
        imgDots[dot][co] = (receive >> 4) & 0b111111111;
        Serial.println();
      }
    }
    clrscr();
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
    printInstruction();
  } else
      Serial.println("Invalid Command");
}