#!/usr/bin/env ruby

=begin

urgency.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2010

ruby-libnotify urgency example

=end

require 'RNotify'

Notify.init("Test5")

test = Notify::Notification.new("Test 5", "urgency test - LOW", nil, nil)
test.urgency= Notify::Notification::URGENCY_LOW
test.timeout= 3000      #3 seconds
test.show

sleep(3)
test.close

test = Notify::Notification.new("Test 5", "urgency test - NORMAL", nil, nil)
test.urgency= Notify::Notification::URGENCY_NORMAL
test.timeout= 3000      #3 seconds
test.show

sleep(3)
test.close

test = Notify::Notification.new("Test 5", "urgency test - CRITICAL", nil, nil)
test.urgency= Notify::Notification::URGENCY_CRITICAL
test.timeout= 3000      #3 seconds
test.show

sleep(3)
test.close

Notify.uninit
