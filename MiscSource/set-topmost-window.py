# file:     set-topmost-window.py - press Ctrl+F11 on a selected window, to topmost/untopmost it
# author:   Ben Mullan 2025
# pre-req:  pip install keyboard pywin32
# exec:     python set-topmost-window.python

import win32gui, win32con, keyboard;

TOPMOST_TOGGLE_HOTKEY = "ctrl+f11";

# toggle-state, for each window
topmost_state = {}

def toggle_topmost():

    hwnd = win32gui.GetForegroundWindow();
    
    if (not hwnd):
        print("No foreground window found.");
        return;

    title = win32gui.GetWindowText(hwnd);
    current_state = topmost_state.get(hwnd, False);

    if current_state:
        # unset topmost
        win32gui.SetWindowPos(
            hwnd,
            win32con.HWND_NOTOPMOST,
            0, 0, 0, 0,
            win32con.SWP_NOMOVE | win32con.SWP_NOSIZE
        );
        print(f"`{title}` is no longer topmost.");
        
    else:
        # set topmost
        win32gui.SetWindowPos(
            hwnd,
            win32con.HWND_TOPMOST,
            0, 0, 0, 0,
            win32con.SWP_NOMOVE | win32con.SWP_NOSIZE
        );
        print(f"`{title}` is now topmost.");

    topmost_state[hwnd] = (not current_state);

# register global hotkey
keyboard.add_hotkey(TOPMOST_TOGGLE_HOTKEY, toggle_topmost);

print(f"Running... Press {TOPMOST_TOGGLE_HOTKEY} to toggle TopMost on the active window. Press ESC to exit.")
keyboard.wait("esc");