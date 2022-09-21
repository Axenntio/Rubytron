def init
	$color_index = 0
	$value = 0
	$initial_width = Window.width
	$initial_height = Window.height
	Window.min_width = 1
	Window.min_height = 1
end

def update
	$value += 0.05
	Window.width = $initial_width + Math.cos($value) * $initial_width / 2
	Window.height = $initial_height + Math.sin($value) * $initial_height / 2
	clear $color_index
	pixel 0, 0, 1
	pixel 0, Window.height - 1, 2
	pixel Window.width - 1, 0, 3
	pixel Window.width - 1, Window.height - 1, 4
end