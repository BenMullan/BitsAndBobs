# file:     simple-remote-desktop-server.py - allows full desktop control + input from browser endpoint; http://localhost:8080/
# pre-req:  pip install flask pillow mss pynput
# exec:     python simple-remote-desktop-server.py
# note:     make endpoint public with eg `cloudflared.exe tunnel --url http://localhost:8080`
# author:   MS-Copilot 2025

import io
import time
import threading
from queue import Queue, Empty
from dataclasses import dataclass

from flask import Flask, Response, request, jsonify, send_from_directory
from PIL import Image
import mss
from pynput.keyboard import Controller as KController, Key
from pynput.mouse import Controller as MController, Button

# -----------------------------
# Config
# -----------------------------
FPS = 12                 # target frames per second for MJPEG
JPEG_QUALITY = 60        # 1-95, higher = better quality, more bandwidth
SCALING = 1.0            # 1.0 = native res; e.g., 0.5 to halve dimensions
BOUNDARY = "frameboundary"
HOST = "127.0.0.1"
PORT = 8080

# -----------------------------
# App and controllers
# -----------------------------
app = Flask(__name__)
keyboard = KController()
mouse = MController()

# -----------------------------
# Capture thread
# -----------------------------
@dataclass
class Frame:
    data: bytes
    ts: float

frame_queue: "Queue[Frame]" = Queue(maxsize=2)
stop_capture = threading.Event()

def capture_loop():
    with mss.mss() as sct:
        monitor = sct.monitors[1]  # primary monitor
        target_interval = 1.0 / max(FPS, 1)

        while not stop_capture.is_set():
            start = time.time()
            shot = sct.grab(monitor)
            img = Image.frombytes("RGB", shot.size, shot.rgb)

            if SCALING != 1.0:
                w = int(img.width * SCALING)
                h = int(img.height * SCALING)
                if w > 0 and h > 0:
                    img = img.resize((w, h), Image.BILINEAR)

            # Encode to JPEG in-memory
            buf = io.BytesIO()
            img.save(buf, format="JPEG", quality=JPEG_QUALITY, optimize=True)
            jpeg_bytes = buf.getvalue()

            # Latest-frame queue (drop older if full)
            try:
                frame_queue.put(Frame(jpeg_bytes, time.time()), timeout=0.01)
            except:
                # If full, try to replace oldest
                try:
                    _ = frame_queue.get_nowait()
                except Empty:
                    pass
                try:
                    frame_queue.put(Frame(jpeg_bytes, time.time()), timeout=0.01)
                except:
                    pass

            # Sleep to match FPS
            elapsed = time.time() - start
            remaining = target_interval - elapsed
            if remaining > 0:
                time.sleep(remaining)

capture_thread = threading.Thread(target=capture_loop, daemon=True)

# -----------------------------
# MJPEG stream route
# -----------------------------
def mjpeg_generator():
    # Start capture if not running
    if not capture_thread.is_alive():
        capture_thread.start()

    # Stream multipart/x-mixed-replace
    while True:
        try:
            frame = frame_queue.get(timeout=1.0)
        except Empty:
            # Heartbeat with last known frameless boundary (avoids client stall)
            yield f"--{BOUNDARY}\r\nContent-Type: image/jpeg\r\nContent-Length: 0\r\n\r\n"
            continue

        yield (
            f"--{BOUNDARY}\r\n"
            f"Content-Type: image/jpeg\r\n"
            f"Content-Length: {len(frame.data)}\r\n\r\n"
        ).encode("utf-8") + frame.data + b"\r\n"

@app.route("/stream")
def stream():
    headers = {
        "Cache-Control": "no-cache, no-store, must-revalidate",
        "Pragma": "no-cache",
        "Expires": "0",
        "Connection": "close",
    }
    return Response(
        mjpeg_generator(),
        mimetype=f"multipart/x-mixed-replace; boundary={BOUNDARY}",
        headers=headers,
    )

# -----------------------------
# Input injection helpers
# -----------------------------
KEY_MAP = {
    # Common special keys
    "enter": Key.enter,
    "return": Key.enter,
    "esc": Key.esc,
    "escape": Key.esc,
    "tab": Key.tab,
    "space": Key.space,
    "backspace": Key.backspace,
    "delete": Key.delete,
    "home": Key.home,
    "end": Key.end,
    "pageup": Key.page_up,
    "pagedown": Key.page_down,
    "left": Key.left,
    "right": Key.right,
    "up": Key.up,
    "down": Key.down,
    "insert": Key.insert,
    "capslock": Key.caps_lock,
    "shift": Key.shift,
    "ctrl": Key.ctrl,
    "control": Key.ctrl,
    "alt": Key.alt,
    "cmd": Key.cmd,
    "win": Key.cmd,
    "meta": Key.cmd,
    # Function keys
    **{f"f{i}": getattr(Key, f"f{i}") for i in range(1, 25)}
}

def press_key(key_str: str):
    k = key_str.lower()
    if k in KEY_MAP:
        keyboard.press(KEY_MAP[k])
        keyboard.release(KEY_MAP[k])
    elif len(key_str) == 1:
        keyboard.press(key_str)
        keyboard.release(key_str)
    else:
        # Unknown key string: best effort type literal
        for ch in key_str:
            keyboard.press(ch)
            keyboard.release(ch)

def key_down(key_str: str):
    k = key_str.lower()
    if k in KEY_MAP:
        keyboard.press(KEY_MAP[k])
    elif len(key_str) == 1:
        keyboard.press(key_str)

def key_up(key_str: str):
    k = key_str.lower()
    if k in KEY_MAP:
        keyboard.release(KEY_MAP[k])
    elif len(key_str) == 1:
        keyboard.release(key_str)

def mouse_button_from_str(btn: str):
    b = (btn or "").lower()
    if b in ("left", "button1", "primary"):
        return Button.left
    if b in ("right", "button2", "secondary"):
        return Button.right
    if b in ("middle", "button3"):
        return Button.middle
    return Button.left

# -----------------------------
# Input endpoint
# -----------------------------
@app.route("/input", methods=["POST"])
def input_events():
    """
    Accepts JSON:
    {
      "events": [
        {"type": "mouse_move", "x": 123, "y": 456},
        {"type": "mouse_down", "button": "left"},
        {"type": "mouse_up", "button": "left"},
        {"type": "mouse_click", "button": "left", "count": 1},
        {"type": "mouse_scroll", "dx": 0, "dy": -5},
        {"type": "key_press", "key": "A"},
        {"type": "key_down", "key": "shift"},
        {"type": "key_up", "key": "shift"},
        {"type": "text", "value": "hello world"}
      ]
    }
    """
    data = request.get_json(silent=True) or {}
    events = data.get("events", [])
    try:
        for ev in events:
            t = ev.get("type")
            if t == "mouse_move":
                x = int(ev.get("x", 0))
                y = int(ev.get("y", 0))
                mouse.position = (x, y)

            elif t == "mouse_down":
                button = mouse_button_from_str(ev.get("button", "left"))
                mouse.press(button)

            elif t == "mouse_up":
                button = mouse_button_from_str(ev.get("button", "left"))
                mouse.release(button)

            elif t == "mouse_click":
                button = mouse_button_from_str(ev.get("button", "left"))
                count = int(ev.get("count", 1))
                for _ in range(max(1, count)):
                    mouse.press(button)
                    mouse.release(button)

            elif t == "mouse_scroll":
                dx = int(ev.get("dx", 0))
                dy = int(ev.get("dy", 0))
                mouse.scroll(dx, dy)

            elif t == "key_press":
                key_str = str(ev.get("key", ""))
                if key_str:
                    press_key(key_str)

            elif t == "key_down":
                key_str = str(ev.get("key", ""))
                if key_str:
                    key_down(key_str)

            elif t == "key_up":
                key_str = str(ev.get("key", ""))
                if key_str:
                    key_up(key_str)

            elif t == "text":
                value = str(ev.get("value", ""))
                for ch in value:
                    keyboard.press(ch)
                    keyboard.release(ch)
        return jsonify({"ok": True})
    except Exception as e:
        return jsonify({"ok": False, "error": str(e)}), 400

# -----------------------------
# Front-end
# -----------------------------
INDEX_HTML = """<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <title>Remote Desktop (MVP)</title>
  <style>
    html, body { height: 100%; margin: 0; background: #111; }
    #container { display: flex; height: 100%; align-items: center; justify-content: center; }
    #view { border: 1px solid #333; background: #000; cursor: crosshair; }
    #toolbar { position: fixed; top: 10px; left: 10px; color: #ddd; font-family: sans-serif; }
    #toolbar button { margin-right: 8px; }
  </style>
</head>
<body>
  <div id="toolbar">
    <button id="toggleInput">Toggle input: <span id="inputState">ON</span></button>
    <span>Click inside the view to focus. ESC toggles input.</span>
  </div>
  <div id="container">
    <img id="view" src="/stream" alt="desktop" />
  </div>
<script>
(function() {
  const img = document.getElementById('view');
  const inputStateEl = document.getElementById('inputState');
  const toggleBtn = document.getElementById('toggleInput');

  let inputEnabled = true;
  const postEvents = async (events) => {
    if (!inputEnabled || events.length === 0) return;
    try {
      await fetch('/input', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ events })
      });
    } catch (e) {
      console.warn('input send failed', e);
    }
  };

  // Toggle input
  const setInputEnabled = (on) => {
    inputEnabled = !!on;
    inputStateEl.textContent = inputEnabled ? 'ON' : 'OFF';
    inputStateEl.style.color = inputEnabled ? '#0f0' : '#f33';
  };
  toggleBtn.addEventListener('click', () => setInputEnabled(!inputEnabled));
  setInputEnabled(true);

  // Keep focus on the image area to capture keys
  img.setAttribute('tabindex', '0');
  img.addEventListener('click', () => img.focus());
  img.addEventListener('keydown', (e) => {
    if (e.key === 'Escape') {
      setInputEnabled(!inputEnabled);
      return;
    }
    e.preventDefault();
    // Prefer special keys by name, else single characters
    postEvents([{ type: 'key_press', key: e.key }]);
  });
  img.addEventListener('keyup', (e) => {
    // Optional: handle key_up if you want true keyDown/up semantics
  });

  // Pointer mapping: map position within image to absolute desktop coords
  function getAbsoluteCoords(ev) {
    const rect = img.getBoundingClientRect();
    const rx = (ev.clientX - rect.left) / rect.width;
    const ry = (ev.clientY - rect.top) / rect.height;

    // We don't know server desktop size here; request a position move as-is.
    // For MVP, we assume the MJPEG image matches desktop resolution.
    // If scaled, precision will be off; can add a /meta endpoint later.
    const width = img.naturalWidth || rect.width;
    const height = img.naturalHeight || rect.height;
    const x = Math.round(rx * width);
    const y = Math.round(ry * height);
    return { x, y };
  }

  img.addEventListener('mousemove', (ev) => {
    const { x, y } = getAbsoluteCoords(ev);
    postEvents([{ type: 'mouse_move', x, y }]);
  });

  img.addEventListener('mousedown', (ev) => {
    ev.preventDefault();
    const button = ev.button === 0 ? 'left' : (ev.button === 1 ? 'middle' : 'right');
    const { x, y } = getAbsoluteCoords(ev);
    postEvents([
      { type: 'mouse_move', x, y },
      { type: 'mouse_down', button }
    ]);
  });

  img.addEventListener('mouseup', (ev) => {
    ev.preventDefault();
    const button = ev.button === 0 ? 'left' : (ev.button === 1 ? 'middle' : 'right');
    postEvents([{ type: 'mouse_up', button }]);
  });

  img.addEventListener('wheel', (ev) => {
    ev.preventDefault();
    // dy positive scrolls up in pynput; browser wheel deltaY is positive when scrolling down
    const dy = -Math.sign(ev.deltaY) * Math.ceil(Math.abs(ev.deltaY) / 100);
    postEvents([{ type: 'mouse_scroll', dx: 0, dy }]);
  });

  // Naive keypress text input (optional): pressing 't' focuses a text capture prompt
  // You can add a text box and send {type:'text', value:'...'} if desired.
})();
</script>
</body>
</html>
"""

@app.route("/")
def index():
    return Response(INDEX_HTML, mimetype="text/html")

# -----------------------------
# Main
# -----------------------------
def main():
    try:
        app.run(host=HOST, port=PORT, threaded=True, debug=False)
    finally:
        stop_capture.set()
        # allow thread to exit
        time.sleep(0.2)

if __name__ == "__main__":
    main()
