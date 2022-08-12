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

def match_app(app, app_desc):
    pid = app.get_pid()
    if pid == 0:
        return app_desc in app.get_name()

    proc = psutil.Process(pid)
    cmd = proc.cmdline()
    if len(cmd) == 0:
        return False

    prog = cmd[0]
    return app_desc in prog


for window in screen.get_windows():
    app = window.get_application()
    if not match_app(app, app_desc):
        continue

    now = GdkX11.x11_get_server_time(Gdk.get_default_root_window())
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
