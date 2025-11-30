# file:     simple-webcam-preview-server.py - serves realtime webcam feed to (browser-viewable) http://localhost:5000/
# pre-req:  pip install flask opencv-python
# exec:     python simple-webcam-preview-server.py
# author:   Ben Mullan, 2025

import flask, cv2;

flaskApp = flask.Flask(__name__);
webcamDevice = cv2.VideoCapture(0);


def generateFrames():
    while True:

        _wasSuccessful, _frameOut = webcamDevice.read();
        if not _wasSuccessful: break;

        _, _buffer = cv2.imencode(".jpg", _frameOut);
        _frameBytes = _buffer.tobytes();

        yield (
            b"--frame\r\n" +
            b"Content-Type: image/jpeg\r\n\r\n" +
            _frameBytes + b"\r\n"
        );


@flaskApp.route("/")
def video_feed():

    return flask.Response(
        generateFrames(),
        mimetype="multipart/x-mixed-replace; boundary=frame"
    );


if __name__ == "__main__":
    flaskApp.run(host="0.0.0.0", port=5000);