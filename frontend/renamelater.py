from flask import Flask, request, jsonify, send_from_directory
import socket

app = Flask(__name__)

# Backend configuration
BACKEND_HOST = '127.0.0.1'
BACKEND_PORT = 8080  # Same as your C++ backend
BUFFER_SIZE = 2048  # Buffer size for receiving data

# Function to send commands to the C++ backend
def send_to_backend(command):
    try:
        # Create a socket connection to the C++ backend
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((BACKEND_HOST, BACKEND_PORT))
            s.sendall(command.encode('utf-8'))
            if "GET" in command:  # Only expect a response for "GET" commands
                response = s.recv(BUFFER_SIZE).decode('utf-8')
                return response
            return "OK"
    except Exception as e:
        return f"Error communicating with backend: {e}"

# Serve the HTML file
@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

# Insert text
@app.route('/insert', methods=['POST'])
def insert():
    data = request.json
    char = data.get('char', '')
    index = data.get('index', 0)
    command = f"INSERT {index} {char}"
    send_to_backend(command)
    return jsonify({"status": "success"})

# Delete text
@app.route('/delete', methods=['POST'])
def delete():
    data = request.json
    index = data.get('index', 0)
    command = f"DELETE {index}"
    send_to_backend(command)
    return jsonify({"status": "success"})

# Fetch the current text
@app.route('/text', methods=['GET'])
def get_text():
    command = "GET"
    text = send_to_backend(command)
    return jsonify({"text": text})

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=True)
