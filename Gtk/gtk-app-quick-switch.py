#!/usr/bin/env python3

import gi
import psutil
import re
import sys
import os

app_regex = None
if len(sys.argv) > 1:
    app_regex = sys.argv[1]

if not app_regex:
    sys.exit(1)

gi.require_version('Gtk', '3.0')
gi.require_version('Wnck', '3.0')
from gi.repository import Gtk, Wnck, GdkX11, Gdk

Gtk.init([])

screen = Wnck.Screen.get_default()
screen.force_update()

if app_regex == '--list':
    for window in screen.get_windows():
        app = window.get_application()
        pid = app.get_pid()
        if pid == 0:
            print(app.get_name())
        else:
            proc = psutil.Process(pid)
            cmd = proc.cmdline()
            print(' '.join(cmd))
            
    sys.exit(0)

def match_app(app, app_regex):
    pid = app.get_pid()

    if pid == 0:
        return re.match(app_regex, app.get_name())

    proc = psutil.Process(pid)
    cmd = proc.cmdline()
    if len(cmd) == 0:
        return False

    prog = ' '.join(cmd)
    return re.search(app_regex, prog, flags=re.IGNORECASE)


for window in screen.get_windows():
    app = window.get_application()
    if not match_app(app, app_regex):
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
if app_regex:
    os.system(app_regex)

sys.exit(0)
