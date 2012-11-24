#!/usr/bin/env ruby

=begin
type=0, DEBUG
type=1, RELEASE
=end

require 'rubygems'

begin
  require 'mkmf-gnome2'
  require 'gtk2'
rescue
  puts "ERROR: missing packages, please install Ruby/Gtk2 and its dependencies",
         "[ http://ruby-gnome2.sourceforge.jp/ ]"
end

$preload = nil
type = 1

def check_set_app_name_libnotify
  if check_required_version("libnotify", 0, 7, 2) == true
    $CFLAGS << ' -DHAVE_SET_APP_NAME'
    puts "with set application name.. yes"
  else
    puts "with set application name.. no"
  end
end

def check_set_notification_name_libnotify
  if check_required_version("libnotify", 0, 7, 3) == true
    $CFLAGS << ' -DHAVE_SET_APP_PNAME'
    puts "with set notification name.. yes"
  else
    puts "with set notification name.. no"
  end
end

def check_required_version(app, maj, min, mic)
  PKGConfig.check_version?(app, maj, min, mic)
end

if check_required_version("libnotify", 0, 7, 0) == true
  if have_library("notify", "notify_init") == true
    $CFLAGS << ' -DDEBUG' if type == 0
    $CFLAGS << ' -Wall' << " -I#{Config::CONFIG["sitearchdir"]} " << PKGConfig.cflags("libnotify")
    $LIBS << ' ' << PKGConfig.libs("libnotify")
    check_set_app_name_libnotify
    check_set_notification_name_libnotify
    create_makefile("rnotify")
  else
     puts "ERROR: please install libnotify",
    "[ http://ftp.gnome.org/pub/GNOME/sources/libnotify ]",
    "[ http://www.galago-project.org/ ]"
  end
else
  puts "libnotify >= 0.7.0 is required in order to use this release."
  puts "if you have libnotify <= 0.6.0 please install ruby-libnotify < 0.5.0"
end
