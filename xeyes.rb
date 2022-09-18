class Vector
	attr_accessor :x
	attr_accessor :y

	def initialize(x, y)
		self.x = x
		self.y = y
	end
end

def init
	$eye_roll = 0
end

def update
	$eye_roll += 0.05
	clear 15
	# Todo: fetch cursor position relative to window
	mouse_pos = Vector.new(0, 0);
	draw_eye(Vector.new(0, 0), mouse_pos)
	draw_eye(Vector.new(Window.width / 2, 0), mouse_pos)
end

def draw_eye(pos, focus_point)
	circle pos.x + 1, pos.y + 1, Window.width / 4 - 1, 7

	eye_x, eye_y = (Math.cos($eye_roll) + 1) * Window.width / 8, (Math.sin($eye_roll) + 1) * Window.width / 8

	circle pos.x + eye_x + 1, pos.y + eye_y + 1, Window.width / 8, 0
end