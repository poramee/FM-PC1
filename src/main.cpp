#include "DataProtocol_PC1.hpp"
#include <TEA5767Radio.h>
#include <Wire.h>

TEA5767Radio radio = TEA5767Radio();

using namespace PC_1;

void interpret(long command);

void setup() {
  Transceiver::init();
  Serial.println("<< PC_1 >>");
  Serial.println("Usage: [1-4]");
  Serial.println("1 Start\n2 Left\n3 Right\n4 Center");
  pinMode(13, OUTPUT);
  radio.setFrequency(105.0);
}

void loop() {
  digitalWrite(13, HIGH);
  if (Serial.available()) {
    const int read = Serial.read() - '0';
    interpret(read);
    Serial.print("Send Completed Successfully");
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
  Serial.print("Start Send with Command = ");
  Serial.println(command);
  if (command == 1) {
    startSend(command);
    long receive = startReceive();
    Serial.print("Receive Message: ");
    for (int i = 15; i >= 0; --i) {
      Serial.print((receive >> i) & 1);
    }
    Serial.println();
    for (int i = 2; i >= 0; --i) {
      const int translate = (receive >> ((i * 3) + 4)) & 7;
      if (translate == 0)
        Serial.println("Bottom");
      else if (translate == 1)
        Serial.println("Top");
      else if (translate == 2)
        Serial.println("Left");
      else if (translate == 3)
        Serial.println("Right");
      else if (translate == 4)
        Serial.println("Upper");
      else if (translate == 5)
        Serial.println("Lower");
    }
  } else if (command >= 2 && command <= 4) {
    startSend(command);
    for (int dot = 0; dot < 16; ++dot) {
      for (int co = 0; co < 3; ++co) {
        long receive = startReceive();
        Serial.print(dot);
        Serial.print(" ");
        Serial.print(co);
        Serial.print(" ");
        Serial.print("Received Message: ");
        for (int i = 15; i >= 0; --i) {
          Serial.print((receive >> i) & 1);
        }
        Serial.println();
      }
    }
  } else
    Serial.println("Invalid Command");
}