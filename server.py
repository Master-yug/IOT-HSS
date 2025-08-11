import firebase_admin
from firebase_admin import credentials, firestore
from flask import Flask, request, jsonify
from datetime import datetime

# Init Flask
app = Flask(__name__)

# Firebase Setup
cred = credentials.Certificate("iot-hss.json")
firebase_admin.initialize_app(cred)
db = firestore.client()

@app.route("/alert", methods=["POST"])
def receive_alert():
    data = request.get_json()
    data["timestamp"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # Save to Firestore under "alerts" collection
    db.collection("alerts").add(data)

    print("📡 Saved to Firebase:", data)
    return jsonify({"status": "success"}), 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
