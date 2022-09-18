def init
	Window.width = 17 * 2
	Window.height = 4
end

def update
	clear 1
	(0..15).each do |i|
		rect i*2+1, 1, 2, 2, i
	end
end