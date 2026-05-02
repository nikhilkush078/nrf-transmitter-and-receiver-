#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

struct ControlData {
  byte direction;  // 0=stop, 1=forward, 2=backward, 3=left, 4=right
  bool button1;
  bool button2;
};

ControlData controls;

const int joyX = A0;
const int joyY = A1;
const int button1 = 4;  // active HIGH
const int button2 = 5;  // active HIGH

void setup() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("=== TRANSMITTER READY ===");
}

void loop() {
  int xValue = analogRead(joyX);
  int yValue = analogRead(joyY);

  controls.direction = 0;  // default stop

  if (yValue > 700)      controls.direction = 1;  // forward
  else if (yValue < 300) controls.direction = 2;  // backward
  else if (xValue < 300) controls.direction = 3;  // left
  else if (xValue > 700) controls.direction = 4;  // right

  controls.button1 = digitalRead(button1);
  controls.button2 = digitalRead(button2);

  radio.write(&controls, sizeof(controls));

  delay(50);
}
