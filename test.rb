class Window
	def self.resize_event(new_width, new_height)
		puts "Received resize event (#{new_width}, #{new_height})"
	end

	def self.close_event
		puts 'Received close event'
	end

	def self.text_event(char)
		puts "Received text event (#{char})"
	end

	def self.focus_event(has_focus)
		puts "Focus changed (#{has_focus})"
		$has_focus = has_focus;
	end
end

def init
	$has_focus = false;
	$color_index = 0
	$value = 0
end

def update
	clear $color_index
	$value += 0.1 if $has_focus
	pixel 0, 0, 1
	pixel 0, Window.height - 1, 2
	pixel Window.width - 1, 0, 3
	pixel Window.width - 1, Window.height - 1, 4
	pixel Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 5
	line 20, 20, Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 9
	pixel Math.cos($value / 2) * 4 + 20, Math.sin($value / 2) * 4 + 20, 6
	line 0, 0, 10, 10, 7
	line 10, 10, 25, 15, 8
	text 1, Window.height - 7, 'Hey! Example demo.'
end