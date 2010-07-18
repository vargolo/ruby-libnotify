#!/usr/bin/env ruby

=begin

update.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2010

ruby-libnotify update example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Notify.init("Test2")

test = Notify::Notification.new("Test 2", "This is a test", nil, nil)
test.timeout= Notify::Notification::EXPIRES_NEVER
test.show

sleep(3)

test.update("Test 2", "Message replaced!", nil)
test.timeout=5000       #5 seconds
test.show

sleep(6)

test.close
Notify.uninit
