#!/usr/bin/env ruby

=begin

base.rb

Luca Russo <vargolo@gmail.com>
Copyright (LGPL) 2006 - 2011

ruby-libnotify basic example

=end

begin
  require 'RNotify'
rescue LoadError
  require 'rubygems'
  require 'RNotify'
end

Notify.init("Test1")
test = Notify::Notification.new("Test 1", "This is a test", nil)
test.timeout= 5000      # 5 seconds
test.show

