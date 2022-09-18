class Vector
	attr_accessor :x
	attr_accessor :y

	def initialize(x, y)
		self.x = x
		self.y = y
	end
end

def update
	clear 15
	# Todo: fetch cursor position relative to window
	mouse_pos = Vector.new(0, 0);
	draw_eye(Vector.new(0, 0), mouse_pos)
	draw_eye(Vector.new(Window.width / 2, 0), mouse_pos)
end

def draw_eye(pos, focus_point)
	circle pos.x, pos.y, Window.width / 4, 7
	# Todo: Handle eye moves
	circle pos.x, pos.y, Window.width / 8, 0
end