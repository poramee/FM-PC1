#include "Transceiver.hpp"
#include <Arduino.h>

namespace PC_1 {
const int receivePin = A0;

extern int sendFrameCount;
extern int receiveFrameCount;
void startSend(int command);
long startReceive();
int makeFrame(int command, int frameNo);
void sendACK();
ACKStatus waitingForACK();
}; // namespace PC_1