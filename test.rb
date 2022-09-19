class Window
	def self.resize_event(new_width, new_height)
		puts "Received resize event (#{new_width}, #{new_height})"
	end

	def self.close_event
		puts 'Received close event'
	end
end

def init
	$color_index = 0
	$value = 0
end

def update
	clear $color_index
	$value += 0.1
	pxl 0, 0, 1
	pxl 0, Window.height - 1, 2
	pxl Window.width - 1, 0, 3
	pxl Window.width - 1, Window.height - 1, 4
	pxl Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 5
	line 20, 20, Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 9
	pxl Math.cos($value / 2) * 4 + 20, Math.sin($value / 2) * 4 + 20, 6
	line 0, 0, 10, 10, 7
	line 10, 10, 25, 15, 8
end