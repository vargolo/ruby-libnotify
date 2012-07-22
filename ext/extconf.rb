#!/usr/bin/env ruby

=begin
type=0, DEBUG
type=1, RELEASE
=end

require 'rubygems'

begin
  require 'mkmf-gnome2'
rescue LoadError
  require 'rubygems'
  gem 'glib2'
  require 'mkmf-gnome2'
end

$preload = nil
type = 1

begin
  require 'gtk2'
rescue LoadError
  puts "ERROR: no gtk2.rb found, please install Ruby/Gtk2",
  "[ http://ruby-gnome2.sourceforge.jp/ ]"
  exit
end

def check_status_icon_gtk2
  if PKGConfig.check_version?("gtk+-2.0", 2, 9, 1) == true
    $CFLAGS << ' -DHAVE_STATUS_ICON'
    puts "with GtkStatusIcon.. yes"
  else
    puts "with GtkStatusIcon.. no"
  end
end

def check_geometry_hints_libnotify
  if PKGConfig.check_version?("libnotify", 0, 4, 1) == true
    $CFLAGS << ' -DHAVE_GEOMETRY_HINTS'
    puts "with geometry hints.. yes"
  else
    puts "with geometry hints.. no"
  end
end

def check_closed_reason_libnotify
  if PKGConfig.check_version?("libnotify", 0, 4, 5) == true
    $CFLAGS << ' -DHAVE_CLOSED_REASON'
    puts "with closed reason.. yes"
  else
    puts "with closed reason.. no"
  end
end

if have_library("notify", "notify_init") == true
  $CFLAGS << ' -DDEBUG' if type == 0
  $CFLAGS << ' -Wall' << " -I#{Config::CONFIG["sitearchdir"]} " << PKGConfig.cflags("libnotify")
  $LIBS << ' ' << PKGConfig.libs("libnotify")
  check_status_icon_gtk2
  check_geometry_hints_libnotify
  check_closed_reason_libnotify
  create_makefile("rnotify")
else
  puts "ERROR: please install libnotify",
  "[ http://www.galago-project.org/ ]"
end
