def init
	$color_index = 1
	$value = 0
end

def update
	clear $color_index
	$value += 0.1
	pxl 0, 0, 2
	pxl 0, 29, 3
	pxl 59, 0, 4
	pxl 59, 29, 5
	pxl Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 6
	line 20, 20, Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 10
	pxl Math.cos($value / 2) * 4 + 20, Math.sin($value / 2) * 4 + 20, 7
	line 0, 0, 10, 10, 8
	line 10, 10, 25, 15, 9
end