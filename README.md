# IOT-HSS

---

## 📌 Overview
The **IOT-HSS** is an end-to-end setup that detects and reports safety-related events (e.g., fire, motion, gas leak) from **hardware sensors** to a **cloud-hosted dashboard**.
- Hardware sensors + ESP-01 module send alerts to a **local server**.
- Local server formats data and sends it to **Firebase Firestore**.
- A **Firebase-hosted secure web dashboard** shows real-time alerts.
- **Only authorized users** can access the dashboard.

---

## 🛠 System Architecture
[ Sensors ] --> [ ESP-01 ] --> [ Local Server ] --> [ Firebase Firestore ] --> [ Firebase Hosting Dashboard ]

---


## Components
1. **Hardware**
   - ESP-01 Wi-Fi module connected to safety sensors (smoke, motion, etc.).
   - Sensors trigger alerts → ESP-01 sends data to the local server over HTTP.

2. **Local Server**
   - Runs on a PC/Raspberry Pi or other small host.
   - Receives HTTP requests from ESP-01.
   - Validates & formats the alert data.
   - Sends it to **Firebase Firestore** using the Firebase Admin SDK.

3. **Cloud Backend (Firebase)**
   - **Firestore Database** stores all alerts with timestamps.
   - **Firebase Authentication** protects access to the dashboard.
   - **Firestore Security Rules** ensure only logged-in users can read/write.

4. **Frontend Dashboard (Firebase Hosting)**
   - Displays latest alerts in real-time (sorted by newest first).
   - Allows login/logout using Firebase Authentication.
   - Only shows alerts to authenticated, authorized users.

---

## Features

### Hardware & Local Server
- Detects safety events from sensors.
- Sends data over Wi-Fi using ESP-01.
- Local server acts as a **bridge** between non-HTTPS ESP-01 and HTTPS Firebase.

### Cloud Backend
- **Firestore** real-time database for alerts.
- Secure read/write rules.
- **Authentication** (email/password) with pre-approved accounts.

### Web Dashboard
- Real-time alert updates without refreshing.
- Alerts sorted by timestamp (newest first).
- Zone and status displayed clearly.
- Mobile and desktop friendly.
- Secure login/logout.
- Prevents public access — **no open registration**.

---

## Hardware Setup
1. Connect your sensors to the ESP-01 module.
2. Flash ESP-01 firmware to:
   - Read sensor values.
   - Send HTTP requests to the local server's IP/Port.
3. Ensure ESP-01 can reach the local server via Wi-Fi.

---

## Local Server Setup
The local server:
- Runs on your PC or Raspberry Pi.
- Receives alerts from ESP-01 over HTTP.
- Sends formatted alerts to Firebase.

## Firebase Setup
1. Create a Firebase Project
    Go to Firebase Console.
    Create a project.
    Enable Firestore Database (production mode).
    Enable Authentication → Email/Password.
    Enable Hosting.

2. Create Firestore Structure
    Collection: alerts

3. Set Security Rules
```
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    match /alerts/{document} {
      allow read, write: if request.auth != null;
    }
  }
}
```
## Dashboard Deployment (Firebase Hosting)
Prerequisites
    Node.js installed.
    Firebase CLI installed:
```
npm install -g firebase-tools
```
Logged in:
```
    firebase login
```
Deployment Steps
```
cd firebase_dashboard
firebase init hosting
# Select your project, public folder: public, single-page app: no
firebase deploy
```
Your dashboard will be available at:

https://<project-id>.web.app

## Authentication
    Only pre-approved users can log in.
    Disable public registration.
    Add authorized users in Firebase Console → Authentication → Users.

## Data Flow
   Sensor triggers → ESP-01 sends alert to local server.
   Local server sends formatted alert to Firestore:
   ``` {
         "zone": "Kitchen",
         "status": "Smoke Detected",
         "timestamp": <Firestore Timestamp>
       } 
   ```
   Dashboard updates in real-time via Firestore listener.

## Known Limitations
  
   ESP-01 cannot send HTTPS requests directly; local server required.
   Timestamps must be stored as Firestore Timestamp objects for proper ordering.
   No offline caching — dashboard requires internet connection.






 
