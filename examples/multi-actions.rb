#!/usr/bin/env ruby

=begin

multi-actions.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2011

ruby-libnotify multi-actions example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Gtk.init
Notify.init("Test4")

test = Notify::Notification.new("Test 4", "multi actions test", nil)
test.timeout= Notify::Notification::EXPIRES_NEVER
test.signal_connect("closed") { Gtk.main_quit }

test.add_action("Click Me action 1", "Click Me! - 1", "bye!! - 1") do |action, data|
  puts "#{action} - #{data}"
  Gtk.main_quit
end

test.add_action("Click Me action 2", "Click Me! - 2", "bye!! - 2") do |action, data|
  puts "#{action} - #{data}"
  Gtk.main_quit
end

test.show

Gtk.main

test.clear_actions
test.close
Notify.uninit
