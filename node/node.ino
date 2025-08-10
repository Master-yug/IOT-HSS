#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

const int pirPin = 2;
bool motionDetected = false;

// Change this to your node ID before uploading
const char* nodeID = "N1"; 

void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  int pirState = digitalRead(pirPin);

  if (pirState == HIGH && !motionDetected) {
    motionDetected = true;
    sendMotionDetected();
  }

  if (pirState == LOW) {
    motionDetected = false; // Reset for next detection
  }

  delay(200); // debounce
}

void sendMotionDetected() {
  char message[8];
  snprintf(message, sizeof(message), "%s:1", nodeID);
  radio.write(&message, sizeof(message));
  Serial.print("Sent: ");
  Serial.println(message);
}
