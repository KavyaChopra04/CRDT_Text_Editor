import socket
from flask import Flask, request

app = Flask(__name__)

# Connect to your existing C++ backend
BACKEND_HOST = '127.0.0.1'
BACKEND_PORT = 8080  # Same as your C++ backend
BUFFER_SIZE = 2048

def send_to_backend(command):
    try:
        # Create a socket connection to the C++ backend
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((BACKEND_HOST, BACKEND_PORT))
            s.sendall(command.encode('utf-8'))
            # Receive response if the command expects one (like GET)
            if command.strip() == "GET":
                response = s.recv(BUFFER_SIZE).decode('utf-8')
                return response
            return "OK"
    except Exception as e:
        return f"Error communicating with backend: {e}"

@app.route('/', methods=['POST'])
def handle_request():
    command = request.data.decode('utf-8')  # Command sent from the frontend
    return send_to_backend(command)

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=8090)
