#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE=9, CSN=10 (your wiring)
const byte address[6] = "00001";

const int pirPin = 2;
bool motionDetected = false;

const char* nodeID = "N1";  // Node ID

void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  Serial.println("Node initialized.");
}

void loop() {
  int pirState = digitalRead(pirPin);

  if (pirState == HIGH && !motionDetected) {
    motionDetected = true;
    sendMotionDetected();
  }

  if (pirState == LOW) {
    motionDetected = false;  // reset
  }

  delay(200); // debounce
}

void sendMotionDetected() {
  char message[8];
  snprintf(message, sizeof(message), "%s:1", nodeID);

  bool ok = radio.write(&message, sizeof(message));
  if (ok) Serial.print("✅ Sent: ");
  else Serial.print("❌ Failed: ");

  Serial.println(message);
}
