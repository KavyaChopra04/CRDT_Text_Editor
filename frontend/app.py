from flask import Flask, request, jsonify, render_template_string

app = Flask(__name__)

HTML_CONTENT = '''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Collaborative Text Editor</title>
    <style>
        body {
            background: linear-gradient(135deg, #0d0d0d, #1a1a1a);
            color: #ffffff;
            font-family: 'Helvetica Neue', Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            padding: 20px;
            box-shadow: inset 0 0 15px rgba(0, 0, 0, 0.5);
        }
        .container {
            background: rgba(30, 30, 30, 0.9);
            border-radius: 12px;
            padding: 20px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.5);
            width: 100%;
            max-width: 800px;
            transition: transform 0.3s ease;
        }
        .container:hover {
            transform: translateY(-5px);
        }
        textarea {
            background-color: #2a2a2a;
            border: none;
            color: #ffffff;
            padding: 20px;
            font-size: 16px;
            border-radius: 8px;
            width: 100%;
            height: 300px;
            outline: none;
            resize: none;
            box-shadow: inset 0 2px 5px rgba(0, 0, 0, 0.5);
            transition: background-color 0.3s ease;
            font-family: 'Courier New', Courier, monospace;
        }
        textarea:focus {
            background-color: #333333;
        }
        #toolbar {
            display: flex;
            justify-content: flex-end;
            margin-bottom: 10px;
        }
        .button {
            background: linear-gradient(90deg, #007aff, #005bb5);
            border: none;
            color: white;
            padding: 10px 20px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            margin-left: 10px;
            cursor: pointer;
            border-radius: 8px;
            transition: background 0.3s ease, transform 0.2s ease;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.3);
        }
        .button:hover {
            background: linear-gradient(90deg, #005bb5, #007aff);
            transform: translateY(-2px);
        }
        .button:active {
            background: linear-gradient(90deg, #004a8d, #005bb5);
            transform: translateY(0);
        }
        .placeholder {
            color: rgba(255, 255, 255, 0.5);
            font-style: italic;
        }
    </style>
</head>
<body>
    <div class="container">
        <div id="toolbar">
            <button class="button" onclick="refreshText()">Refresh</button>
        </div>
        <textarea id="text-editor" placeholder="Start typing..."></textarea>
    </div>
    <script>
        const editor = document.getElementById('text-editor');

        function sendData(url, data) {
            fetch(url, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
            })
            .then(response => response.json())
            .then(data => {
                editor.value = data.text;
            });
        }

        editor.addEventListener('input', (event) => {
            const inputType = event.inputType;
            const data = event.data;
            const selectionStart = editor.selectionStart;

            if (inputType === 'insertText' || inputType === 'insertFromPaste') {
                // Insert text
                sendData('http://127.0.0.1:5000/insert', {
                    char: data || "",
                    index: selectionStart - (data?.length || 1)
                });
            } else if (inputType === 'deleteContentBackward') {
                // Delete text
                sendData('http://127.0.0.1:5000/delete', {
                    index: selectionStart
                });
            } else if (inputType === 'insertLineBreak') {
                // Handle Enter key
                sendData('http://127.0.0.1:5000/insert', {
                    char: '\\n',
                    index: selectionStart - 1
                });
            }
        });

        function refreshText() {
            fetch('http://127.0.0.1:5000/text')
            .then(response => response.json())
            .then(data => {
                editor.value = data.text;
            });
        }

        // Periodically fetch text
        setInterval(refreshText, 10);
    </script>
</body>
</html>
'''

# Initialize text storage
text_storage = ""

@app.before_request
def log_request():
    data = request.get_json() if request.is_json else "No JSON payload"
    if request.path != '/text':
        print(f"Received {request.method} request on {request.path}. Payload: {data}")

@app.route('/')
def index():
    return render_template_string(HTML_CONTENT)

@app.route('/insert', methods=['POST'])
def insert():
    global text_storage
    char = request.json.get('char', '')
    index = request.json.get('index', len(text_storage))
    if 0 <= index <= len(text_storage):
        text_storage = text_storage[:index] + char + text_storage[index:]
    return jsonify({'text': text_storage})

@app.route('/delete', methods=['POST'])
def delete():
    global text_storage
    index = request.json.get('index', len(text_storage))
    if 0 <= index < len(text_storage):
        text_storage = text_storage[:index] + text_storage[index + 1:]
    return jsonify({'text': text_storage})

@app.route('/text', methods=['GET'])
def get_text():
    print(f"Sending text to client: {text_storage}")
    return jsonify({'text': text_storage})

if __name__ == '__main__':
    app.run(debug=True)