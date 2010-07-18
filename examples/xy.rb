#!/usr/bin/env ruby

=begin

xy.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2010

ruby-libnotify xy example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Notify.init("Test7")

test = Notify::Notification.new("Test 7", "This notification point to 150,100", nil, nil)
test.hint32("x", 150)
test.hint32("y", 100)
test.timeout= 5000      #5 seconds
test.show

sleep(6)
test.clear_hints
test.close
Notify.uninit
