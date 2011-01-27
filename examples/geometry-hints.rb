#!/usr/bin/env ruby

=begin

geometry-hints.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2011

ruby-libnotify geometry-hints example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Gtk.init
Notify.init("Test10")

screen = Gdk::Screen.default

test = Notify::Notification.new("Test10", "This notification point to 150,100", nil, nil)
test.geometry_hints(screen, 150, 100)
test.timeout= Notify::Notification::EXPIRES_NEVER
test.add_action("Action", "Click Me!") { Gtk.main_quit }
test.signal_connect("closed") { Gtk.main_quit }
test.show

Gtk.main

test.clear_actions
test.close
Notify.uninit
