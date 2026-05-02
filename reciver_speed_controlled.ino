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

// LEFT MOTOR
const int leftPWM = 3;
const int leftIN1 = 2;
const int leftIN2 = 4;

// RIGHT MOTOR
const int rightPWM = 5;
const int rightIN1 = 6;
const int rightIN2 = 7;

const int commLED = 8;
unsigned long lastReceiveTime = 0;

void stopAll() {
  analogWrite(leftPWM, 0);
  analogWrite(rightPWM, 0);

  digitalWrite(leftIN1, LOW);
  digitalWrite(leftIN2, LOW);
  digitalWrite(rightIN1, LOW);
  digitalWrite(rightIN2, LOW);
}

void setup() {

  pinMode(leftPWM, OUTPUT);
  pinMode(leftIN1, OUTPUT);
  pinMode(leftIN2, OUTPUT);
  pinMode(rightPWM, OUTPUT);
  pinMode(rightIN1, OUTPUT);
  pinMode(rightIN2, OUTPUT);
  pinMode(commLED, OUTPUT);

  stopAll();

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {

  if (radio.available()) {

    radio.read(&controls, sizeof(controls));
    lastReceiveTime = millis();
    digitalWrite(commLED, HIGH);

    int pwmValue = controls.speed;

    switch (controls.direction) {

      case 1: // Forward
        digitalWrite(leftIN1, HIGH);
        digitalWrite(leftIN2, LOW);
        digitalWrite(rightIN1, HIGH);
        digitalWrite(rightIN2, LOW);
        break;

      case 2: // Backward
        digitalWrite(leftIN1, LOW);
        digitalWrite(leftIN2, HIGH);
        digitalWrite(rightIN1, LOW);
        digitalWrite(rightIN2, HIGH);
        break;

      case 3: // Right
        digitalWrite(leftIN1, HIGH);
        digitalWrite(leftIN2, LOW);
        digitalWrite(rightIN1, LOW);
        digitalWrite(rightIN2, HIGH);
        break;

      case 4: // Left
        digitalWrite(leftIN1, LOW);
        digitalWrite(leftIN2, HIGH);
        digitalWrite(rightIN1, HIGH);
        digitalWrite(rightIN2, LOW);
        break;

      default:
        stopAll();
        return;
    }

    analogWrite(leftPWM, pwmValue);
    analogWrite(rightPWM, pwmValue);
  }

  // Fail-safe
  if (millis() - lastReceiveTime > 1000) {
    stopAll();
    digitalWrite(commLED, LOW);
  }
}
