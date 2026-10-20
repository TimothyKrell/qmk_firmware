#!/usr/bin/env python3
"""Keymap overlay daemon. Shows keymap-screen.svg on a wlr-layer-shell overlay over the focused output, so tiled
windows don't move. It stays resident with the SVG already loaded and toggles on SIGUSR1, so showing it is instant.
Reloads when draw.sh rewrites the SVG. Any non-modifier key or a click hides it.

Started by show-keymap.sh (niri runs it with --daemon at login), which preloads libgtk4-layer-shell
(it has to load before libwayland)."""
import os
import signal
import sys

import gi

gi.require_version("Gdk", "4.0")
gi.require_version("Gtk", "4.0")
gi.require_version("Gtk4LayerShell", "1.0")
from gi.repository import Gdk, Gio, GLib, Gtk, Gtk4LayerShell as LayerShell  # noqa: E402

APP_ID = "dev.timothykrell.KeymapViewer"
SVG = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "keymap-screen.svg")
CSS = b"window { background: rgba(0, 0, 0, 0.85); } picture { margin: 24px; }"
PIDFILE = os.path.join(os.environ.get("XDG_RUNTIME_DIR", "/tmp"), "keymap-overlay.pid")


def on_activate(app):
    app.hold()  # keep running while hidden
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

    def on_key(ctrl, *_):
        # Ignore bare modifiers, so pressing Mod for Mod+/ leaves the hiding to niri's toggle.
        if not ctrl.get_current_event().is_modifier():
            win.set_visible(False)

    keys = Gtk.EventControllerKey()
    keys.connect("key-pressed", on_key)
    win.add_controller(keys)
    click = Gtk.GestureClick()
    click.connect("released", lambda *_: win.set_visible(False))
    win.add_controller(click)
    win.connect("close-request", lambda *_: win.set_visible(False) or True)

    css = Gtk.CssProvider()
    css.load_from_data(CSS)
    Gtk.StyleContext.add_provider_for_display(Gdk.Display.get_default(), css, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
    LayerShell.init_for_window(win)
    LayerShell.set_namespace(win, "keymap-overlay")
    LayerShell.set_layer(win, LayerShell.Layer.OVERLAY)
    for edge in (LayerShell.Edge.TOP, LayerShell.Edge.BOTTOM, LayerShell.Edge.LEFT, LayerShell.Edge.RIGHT):
        LayerShell.set_anchor(win, edge, True)
    LayerShell.set_exclusive_zone(win, -1)  # cover panels too
    LayerShell.set_keyboard_mode(win, LayerShell.KeyboardMode.EXCLUSIVE)

    def toggle():
        win.set_visible(not win.get_visible())
        return GLib.SOURCE_CONTINUE

    GLib.unix_signal_add(GLib.PRIORITY_HIGH, signal.SIGUSR1, toggle)
    with open(PIDFILE, "w") as f:
        f.write(str(os.getpid()))
    if "--daemon" not in sys.argv:
        win.present()
        return

    # The first map costs ~0.5s (renderer setup, texture upload). Pay it now with one invisible,
    # non-focusing frame so the first Mod+/ is as fast as the rest.
    def warmed_up():
        win.set_visible(False)
        win.set_opacity(1)
        LayerShell.set_keyboard_mode(win, LayerShell.KeyboardMode.EXCLUSIVE)
        return GLib.SOURCE_REMOVE

    def on_first_map(*_):
        win.disconnect(handler)
        GLib.timeout_add(150, warmed_up)

    LayerShell.set_keyboard_mode(win, LayerShell.KeyboardMode.NONE)
    win.set_opacity(0)
    handler = win.connect("map", on_first_map)
    win.present()


app = Gtk.Application(application_id=APP_ID, flags=Gio.ApplicationFlags.NON_UNIQUE)
app.connect("activate", on_activate)
sys.exit(app.run([]))
