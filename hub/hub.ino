#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <time.h>  // For NTP time

// ---------------- Wi-Fi ----------------
#define WIFI_SSID     "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// ---------------- Firebase ----------------
#define FIREBASE_HOST "FIREBASE_HOST"
#define FIREBASE_AUTH "FIREBASE_AUTH"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- RF24 ----------------
RF24 radio(13, 12);  // CE=13, CSN=12
const byte address[6] = "00001";

// ---------------- Node-Zone Mapping ----------------
String getZone(String nodeId) {
  if (nodeId == "N1") return "Living Room";
  if (nodeId == "N2") return "Bedroom";
  if (nodeId == "N3") return "Kitchen";
  return "Unknown";
}

// ---------------- Timestamp ----------------
String getTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "Unknown";
  char buf[20];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buf);
}

// ---------------- Timer for printing NTP time ----------------
unsigned long lastTimePrint = 0;
const unsigned long printInterval = 30000; // 30 seconds

void setup() {
  Serial.begin(115200);

  // Connect Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  // NTP time setup
  const char* ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = 19800; // IST +5:30
  const int daylightOffset_sec = 0;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Synchronizing time...");
  struct tm timeinfo;
  while(!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nTime synchronized.");

  // Firebase setup
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase initialized.");

  // nRF24 setup
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  Serial.println("RF24 receiver ready.");
}

void loop() {
  // ---------------- Print NTP time periodically ----------------
  if (millis() - lastTimePrint > printInterval) {
    Serial.println("Current NTP time: " + getTimestamp());
    lastTimePrint = millis();
  }

  // ---------------- RF24 data reception ----------------
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));

    Serial.print("Received raw: ");
    Serial.println(text);

    // Parse "N1:1"
    String payload = String(text);
    String nodeId = payload.substring(0, payload.indexOf(":"));
    String statusCode = payload.substring(payload.indexOf(":") + 1);

    if (statusCode == "1") {
      String status = "Motion Detected";
      String zone = getZone(nodeId);
      String ts = getTimestamp();

      FirebaseJson json;
      json.add("node", nodeId);
      json.add("status", status);
      json.add("zone", zone);
      json.add("timestamp", ts);

      if (Firebase.pushJSON(fbdo, "/detections", json)) {
        Serial.println("✅ Uploaded to Firebase");
        Serial.println(fbdo.payload());
      } else {
        Serial.print("❌ Firebase error: ");
        Serial.println(fbdo.errorReason());
      }
    }
  }
}
