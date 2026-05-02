#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

struct ControlData {
  byte direction;
  bool button1;
  bool button2;
};

ControlData controls;

// Motor direction output pins
const int forwardPin  = 2;
const int backwardPin = 3;
const int leftPin     = 4;
const int rightPin    = 5;

// Button output pins (toggle)
const int button1Pin = 6;
const int button2Pin = 7;

// Communication LED (ON = connected)
const int commLED = 8;

// Variables for toggles
bool button1State = LOW;
bool button2State = LOW;

unsigned long lastReceiveTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
  pinMode(button1Pin, OUTPUT);
  pinMode(button2Pin, OUTPUT);
  pinMode(commLED, OUTPUT);

  stopAll();

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("=== RECEIVER READY ===");
}

void stopAll() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, LOW);
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, LOW);
}

void loop() {
  if (radio.available()) {
    radio.read(&controls, sizeof(controls));
    lastReceiveTime = millis();
    digitalWrite(commLED, HIGH);  // Communication stable LED ON

    // Reset all directions
    stopAll();

    switch (controls.direction) {
      case 1: digitalWrite(forwardPin, HIGH); break;
      case 2: digitalWrite(backwardPin, HIGH); break;
      case 3: digitalWrite(leftPin, HIGH); break;
      case 4: digitalWrite(rightPin, HIGH); break;
      default: stopAll(); break;
    }

    // --- Toggle button logic ---
    static bool prevButton1 = LOW;
    static bool prevButton2 = LOW;

    if (controls.button1 && !prevButton1) {
      button1State = !button1State;
      digitalWrite(button1Pin, button1State);
    }
    prevButton1 = controls.button1;

    if (controls.button2 && !prevButton2) {
      button2State = !button2State;
      digitalWrite(button2Pin, button2State);
    }
    prevButton2 = controls.button2;

    // Debug info
    Serial.print("Dir: "); Serial.print(controls.direction);
    Serial.print(" | B1: "); Serial.print(button1State ? "ON" : "OFF");
    Serial.print(" | B2: "); Serial.println(button2State ? "ON" : "OFF");
  }

  // If no signal for 1 second → turn off communication LED
  if (millis() - lastReceiveTime > 1000) {
    digitalWrite(commLED, LOW);
    stopAll();
  }
}
