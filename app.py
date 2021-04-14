# codinf:utf-8
from flask import Flask, render_template, Response, request
import serial

port = "COM3"
baud = 115200
app = Flask(__name__)
serialInst = serial.Serial(port, baud)


@app.route('/switch', methods=['POST'])
def switch():
    print(request.form['id'], request.form['state'])
    return 'button switched'


@app.route('/')
def index():
    return render_template('main.html')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True, threaded=True)
