#!/usr/bin/env ruby

=begin

action.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2011

ruby-libnotify action example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Gtk.init
Notify.init("Test3")

test = Notify::Notification.new("Test 3", "action test", nil)
test.timeout= Notify::Notification::EXPIRES_NEVER
test.signal_connect("closed") { Gtk.main_quit }

test.add_action("Click Me action", "Click Me!", "bye!!") do |action, data|
  puts "#{action} - #{data}"
  Gtk.main_quit
end

test.show

Gtk.main

test.clear_actions
test.close
Notify.uninit
