#!/usr/bin/env ruby

=begin

attach_to_status_icon.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2010

ruby-libnotify attach_to_status_icon example

=end

require 'RNotify'

Gtk.init
Notify.init("Test11")

win = Gtk::Window.new(Gtk::Window::TOPLEVEL)
win.signal_connect("destroy") { Gtk.main_quit }

bt = Gtk::Button.new("Click Me!")

win.add(bt)
win.show_all

icon = Gdk::Pixbuf.new(File.dirname(__FILE__) + "/icon.png")
st = Gtk::StatusIcon.new
st.pixbuf=icon

test = Notify::Notification.new("Attach_To", "Do you savvy? :P", nil, st)
test.timeout= Notify::Notification::EXPIRES_NEVER
test.pixbuf_icon=icon
test.add_action("Attach_To", "Click Me!") { Gtk.main_quit }

bt.signal_connect("clicked", test) { |wid,data| data.show }

Gtk.main

test.clear_actions
test.close
Notify.uninit
