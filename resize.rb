def init
	$color_index = 1
	$value = 0
	$initial_width = Window.width
	$initial_height = Window.height
end

def update
	$value += 0.05
	Window.width = $initial_width + Math.cos($value) * $initial_width / 2
	Window.height = $initial_height + Math.sin($value) * $initial_height / 2
	clear $color_index
	pxl 0, 0, 2
	pxl 0, Window.height - 1, 3
	pxl Window.width - 1, 0, 4
	pxl Window.width - 1, Window.height - 1, 5
end