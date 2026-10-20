#!/usr/bin/env python3
"""Full-screen keymap viewer. Shows keymap-screen.svg scaled to fit and reloads it when draw.sh rewrites it.
Any key or click closes it. Launched (and toggled) by show-keymap.sh."""
import os
import sys

import gi

gi.require_version("Gdk", "4.0")
gi.require_version("Gtk", "4.0")
from gi.repository import Gdk, Gio, GLib, Gtk  # noqa: E402

APP_ID = "dev.timothykrell.KeymapViewer"
SVG = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "keymap-screen.svg")
CSS = b"window { background: #000; } picture { margin: 24px; }"


def on_activate(app):
    win = Gtk.ApplicationWindow(application=app, title="Kyria keymap")
    picture = Gtk.Picture(content_fit=Gtk.ContentFit.CONTAIN, can_shrink=True)
    win.set_child(picture)

    def load(*_):
        try:
            picture.set_paintable(Gdk.Texture.new_from_filename(SVG))
        except GLib.Error as e:
            print(f"keymap-viewer: {e.message}", file=sys.stderr)

    load()
    # draw.sh replaces the file in place; debounce the burst of change events.
    monitor = Gio.File.new_for_path(SVG).monitor_file(Gio.FileMonitorFlags.NONE, None)
    monitor.connect("changed", lambda *_: GLib.timeout_add(300, lambda: load() or False))
    win.monitor = monitor

    keys = Gtk.EventControllerKey()
    keys.connect("key-pressed", lambda *_: win.close())
    win.add_controller(keys)
    click = Gtk.GestureClick()
    click.connect("released", lambda *_: win.close())
    win.add_controller(click)

    css = Gtk.CssProvider()
    css.load_from_data(CSS)
    Gtk.StyleContext.add_provider_for_display(Gdk.Display.get_default(), css, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
    win.fullscreen()
    win.present()


app = Gtk.Application(application_id=APP_ID, flags=Gio.ApplicationFlags.NON_UNIQUE)
app.connect("activate", on_activate)
sys.exit(app.run([]))
