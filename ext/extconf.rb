#!/usr/bin/env ruby

=begin
type=0, DEBUG
type=1, RELEASE
=end

require 'rubygems'

begin
  require 'pkg-config'
rescue LoadError
  puts "ERROR: no pkg-config.rb file found, please install pkg-config",
         "[ http://rubygems.org/gems/pkg-config ]"
  exit
end

$preload = nil
type = 1

def find_gtk2_rb
  (File.exist?("#{Config::CONFIG["sitelibdir"]}/gtk2.rb") ||
   File.exist?("#{Config::CONFIG["rubylibdir"]}/gtk2.rb"))
end

def check_set_app_name_libnotify
  if PKGConfig.check_version?("libnotify", 0, 7, 2) == true
    $CFLAGS << ' -DHAVE_SET_APP_NAME'
    puts "with set application name.. yes"
  else
    puts "with set application name.. no"
  end
end

def check_required_version(app, maj, min, mic)
  PKGConfig.check_version?(app, maj, min, mic)
end

if check_required_version("libnotify", 0, 7, 0) == true
  if find_gtk2_rb == true
    if have_library("notify", "notify_init") == true
      $CFLAGS << ' -DDEBUG' if type == 0
      $CFLAGS << ' -Wall' << " -I#{Config::CONFIG["sitearchdir"]} " << PKGConfig.cflags("libnotify")
      $LIBS << ' ' << PKGConfig.libs("libnotify")
      check_set_app_name_libnotify
      create_makefile("rnotify")
    else
      puts "ERROR: please install libnotify",
	    "[ http://www.galago-project.org/ ]"
    end
  else
    puts "ERROR: no gtk2.rb found, please install Ruby/Gtk2",
         "[ http://ruby-gnome2.sourceforge.jp/ ]"
  end
else
  puts "libnotify >= 0.7.0 is required in order to use this release."
  puts "if you have libnotify <= 0.6.0 please install ruby-libnotify < 0.5.0"
end
