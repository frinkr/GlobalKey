#!/usr/bin/env python3

import gi
import psutil
import sys
import os

app_desc = None
app_cmd = None
if len(sys.argv) > 1:
    app_desc = sys.argv[1]

if len(sys.argv) > 2:
    app_cmd = sys.argv[2]

if not app_desc:
    sys.exit(1)

gi.require_version('Gtk', '3.0')
gi.require_version('Wnck', '3.0')
from gi.repository import Gtk, Wnck, GdkX11, Gdk

Gtk.init([])

screen = Wnck.Screen.get_default()
screen.force_update()

for window in screen.get_windows():
    app = window.get_application()
    pid = app.get_pid()
    if pid == 0:
        continue

    proc = psutil.Process(pid)
    cmd = proc.cmdline()
    if len(cmd) == 0:
        continue

    now = GdkX11.x11_get_server_time(Gdk.get_default_root_window())

    prog = cmd[0]
    if app_desc in prog:
        if window.is_minimized():
            window.activate(now)
        else:
            if window.is_active():
                window.minimize()
            else:
                window.activate(now)
        sys.exit(0)


# window not found, launch the app
if app_cmd:
    os.system(app_cmd)

sys.exit(0)
