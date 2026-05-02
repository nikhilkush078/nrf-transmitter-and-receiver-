#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
const byte address[6] = "00001";

struct ControlData {
  int speed;
  int direction;
};

ControlData controls;

const int joyY = A0;  // Forward/Backward
const int joyX = A1;  // Left/Right
const int commLED = 8;

void setup() {

  pinMode(commLED, OUTPUT);
  digitalWrite(commLED, LOW);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);
  radio.openWritingPipe(address);
  radio.setAutoAck(true);
  radio.stopListening();
}

void loop() {

  int yValue = analogRead(joyY);
  int xValue = analogRead(joyX);

  controls.speed = 0;
  controls.direction = 0;

  // Forward
  if (yValue > 550) {
    controls.direction = 1;
    controls.speed = map(yValue, 550, 1023, 0, 255);
  }

  // Backward
  else if (yValue < 470) {
    controls.direction = 2;
    controls.speed = map(yValue, 470, 0, 0, 255);
  }

  // Right
  else if (xValue > 550) {
    controls.direction = 3;
    controls.speed = map(xValue, 550, 1023, 0, 255);
  }

  // Left
  else if (xValue < 470) {
    controls.direction = 4;
    controls.speed = map(xValue, 470, 0, 0, 255);
  }

  bool success = radio.write(&controls, sizeof(controls));
  digitalWrite(commLED, success ? HIGH : LOW);

  delay(20);
}
