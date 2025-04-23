#!/usr/bin/env ruby

puts "Content-Type: text/plain"
puts
puts "Starting infinite loop..."

begin
  while true
	puts "Still running..."
	sleep(1)
  thisISAnError()
  end
rescue Interrupt
  puts "Infinite loop interrupted."
end

puts "Exiting infinite loop."