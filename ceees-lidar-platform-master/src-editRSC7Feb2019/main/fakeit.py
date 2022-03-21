import subprocess

from flask import Flask
app = Flask(__name__)

running = False
p = None

def start():
    global running
    if running:
        return
    running = True

    global p
    p = subprocess.Popen(
            './main'
    )

def stop():
    global running
    if not running:
        return
    running = False

    global p
    if p:
        p.kill()

@app.route("/status/", methods=["GET"])
def poll_status():
    if running:
        return "Running"
    else:
        return "Stopped"

@app.route("/status/<state>", methods=["GET"])
def set_status(state):
    if state == "stop":
        stop()
        return "Stopped"
    elif state == "start":
        start()
        return "Running"


if __name__ == "__main__":
    app.run('0.0.0.0', port=8000)



