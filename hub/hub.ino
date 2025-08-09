#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>

// ====== NRF24L01 Setup ======
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

// ====== ESP-01 Setup ======
SoftwareSerial esp(7, 8); // RX, TX
String wifiSSID = "Chaudhari";
String wifiPASS = "9632899161";

// Flask Server IP & Port
String serverIP = "192.168.68.107";
String serverPort = "5000";

// ====== Node to Zone Mapping ======
String mapNodeToZone(const String &nodeID) {
  if (nodeID == "N1") return "Living Room";
  if (nodeID == "N2") return "Garage";
  if (nodeID == "N3") return "Kitchen";
  return "Unknown Zone";
}

void sendToESP(String cmd, int delayMs) {
  esp.println(cmd);
  delay(delayMs);
  while (esp.available()) {
    Serial.write(esp.read());
  }
}

void setup() {
  Serial.begin(9600);
  esp.begin(9600);

  Serial.println("=== Arduino Uno RF Receiver with ESP-01 JSON POST (Motion Only) ===");

  // NRF24 Init
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // ESP8266 Init
  sendToESP("AT", 1000);
  sendToESP("AT+CWMODE=1", 1000);
  sendToESP("AT+CWJAP=\"" + wifiSSID + "\",\"" + wifiPASS + "\"", 5000);
}

void loop() {
  if (radio.available()) {
    char text[32] = {0};
    radio.read(&text, sizeof(text));

    String rfMessage = String(text);
    Serial.println("RF Received: " + rfMessage);

    int sepIndex = rfMessage.indexOf(':');
    if (sepIndex > 0) {
      String nodeID = rfMessage.substring(0, sepIndex);
      String motion = rfMessage.substring(sepIndex + 1);

      // Only handle motion detected ("1")
      if (motion == "1") {
        String zone = mapNodeToZone(nodeID);
        String status = "Motion Detected";

        // JSON payload
        String jsonData = "{\"node\":\"" + nodeID + "\",\"zone\":\"" + zone + "\",\"status\":\"" + status + "\"}";

        // HTTP POST Request
        String postRequest =
          "POST /alert HTTP/1.1\r\n"
          "Host: " + serverIP + ":" + serverPort + "\r\n"
          "Content-Type: application/json\r\n"
          "Content-Length: " + String(jsonData.length()) + "\r\n"
          "Connection: close\r\n"
          "\r\n" + jsonData;

        // Send via ESP-01
        sendToESP("AT+CIPSTART=\"TCP\",\"" + serverIP + "\"," + serverPort, 2000);
        sendToESP("AT+CIPSEND=" + String(postRequest.length()), 1000);
        esp.print(postRequest);
        delay(1500);
        sendToESP("AT+CIPCLOSE", 500);
      }
    }
  }
}
