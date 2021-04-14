# codinf:utf-8
from flask import Flask, render_template, Response, request
import serial

port = "COM3"
baud = 115200
portDict = {
    'sw0': ['a', '0'],
    'sw1': ['a', '1'],
    'sw2': ['a', '2'],
    'sw3': ['a', '3'],
    'sw4': ['a', '4'],
    'sw5': ['a', '5'],
    'sw6': ['a', '6'],
    'sw7': ['a', '7'],
}
app = Flask(__name__)
serialInst = serial.Serial(port, baud)


@app.route('/switch', methods=['POST'])
def switch():
    id = str(request.form['id'])
    st = str(request.form['state'])
    print(id, st)
    port = portDict[id]
    cmd = 'w,' + port[0] + ',' + port[1] + ',' + '>'
    return 'button switched'


@app.route('/')
def index():
    return render_template('main.html')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080, debug=True, threaded=True)
