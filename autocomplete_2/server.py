from flask import Flask, request, jsonify, render_template
import subprocess

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/autocomplete', methods=['POST'])
def autocomplete():
    data = request.get_json()
    prefix = data.get('prefix', '')

    # Write the prefix to input.txt for the C++ program
    with open('input.txt', 'w') as f:
        f.write(prefix)

    # Run the C++ program
    subprocess.run(['./autocomplete'], check=True)

    # Read suggestions from output.txt
    suggestions = []
    with open('output.txt', 'r') as f:
        suggestions = [line.strip() for line in f.readlines()]

    return jsonify(suggestions)

if __name__ == '__main__':
    app.run(debug=True)
