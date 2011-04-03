#!/usr/bin/env ruby

=begin

urgency.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2011

ruby-libnotify urgency example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Notify.init("Test5")

test = Notify::Notification.new("Test 5", "urgency test - LOW", nil)
test.urgency= Notify::Notification::URGENCY_LOW
test.timeout= 3000      #3 seconds
test.show

sleep(3)
test.close

test = Notify::Notification.new("Test 5", "urgency test - NORMAL", nil)
test.urgency= Notify::Notification::URGENCY_NORMAL
test.timeout= 3000      #3 seconds
test.show

sleep(3)
test.close

test = Notify::Notification.new("Test 5", "urgency test - CRITICAL", nil)
test.urgency= Notify::Notification::URGENCY_CRITICAL
test.timeout= 3000      #3 seconds
test.show
