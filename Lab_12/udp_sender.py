import requests
import time
import random
from flask import Flask, jsonify, request

# Completati cu adresa IP a platformei ESP32
PEER_IP = "192.168.4.1"
PEER_PORT = 80

app = Flask(__name__)

# Temp Mock
def get_temperature():
    return random.randint(20, 30)

# State Mock
def get_esp32_state():
    return random.choice(["ON", "OFF"])

# Config Mock
def get_esp32_config():
    return {
        "threshold": random.randint(25, 35),
        "interval": random.randint(1, 10)
    }

@app.route('/temp', methods=['GET'])
def get_temperature_route():
    return jsonify({'Internal Temp': get_temperature()})

@app.route('/state', methods=['GET'])
def get_esp32_state_route():
    return jsonify({'state': get_esp32_state(), 'Internal Temp': get_temperature()})

@app.route('/config', methods=['GET', 'POST', 'PUT', 'DELETE'])
def esp32_config_route():
    if request.method == 'GET':
        return jsonify(get_esp32_config())
    elif request.method == 'POST':
        # POST logic
        return "POST detected"
    elif request.method == 'PUT':
        # PUT logic
        return "PUT detected"
    elif request.method == 'DELETE':
        # DELETE logic
        return "DELETE detected"

def send_request(method, url, data=None):
    try:
        response = requests.request(method, url, json=data)
        print(f"{method} request sent to {url}")
        print("Response:", response.text)
    except Exception as e:
        print("Error occurred:", e)

while 1:
    try:
        # Send GET requests
        send_request('GET', f"http://{PEER_IP}:{PEER_PORT}/temp")
        send_request('GET', f"http://{PEER_IP}:{PEER_PORT}/state")
        send_request('GET', f"http://{PEER_IP}:{PEER_PORT}/config")

        # Send POST, PUT, DELETE requests
        send_request('POST', f"http://{PEER_IP}:{PEER_PORT}/config", data={"key": "value"})
        send_request('PUT', f"http://{PEER_IP}:{PEER_PORT}/config", data={"key": "value"})
        send_request('DELETE', f"http://{PEER_IP}:{PEER_PORT}/config")

        time.sleep(1)
    except KeyboardInterrupt:
        break
