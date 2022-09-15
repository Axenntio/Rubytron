def init
	$color_index = 2
end

def update
	clear $color_index
	line 0, 0, 10, 10
	line 10, 10, 25, 15
	pxl 5, 10
end