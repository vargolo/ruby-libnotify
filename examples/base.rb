#!/usr/bin/env ruby

=begin

base.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2010

ruby-libnotify basic example

=end

require 'RNotify'

Notify.init("Test1")
test = Notify::Notification.new("Test 1", "This is a test", nil, nil)
test.timeout= 5000      # 5 seconds
test.show

sleep(5)

test.close
Notify.uninit
