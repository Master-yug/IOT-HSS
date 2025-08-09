#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN pins for Nano
const byte address[6] = "00001";

const int pirPin = 2; // PIR sensor pin
bool motionSent = false;

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

  // Only send once when motion is first detected
  if (pirState == HIGH && !motionSent) {
    motionSent = true;
    sendMotionDetected();
  }
  // Reset flag when motion stops (ready for next detection)
  else if (pirState == LOW) {
    motionSent = false;
  }
}

void sendMotionDetected() {
  char message[6];
  snprintf(message, sizeof(message), "N1:1"); // Example "N1:1" for motion
  radio.write(&message, sizeof(message));
  Serial.print("Sent: ");
  Serial.println(message);
}
