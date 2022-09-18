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
	mouse_pos = Vector.new(Window.mouse_x, Window.mouse_y);
	draw_eye(Vector.new(0, 0), mouse_pos)
	draw_eye(Vector.new(Window.width / 2, 0), mouse_pos)
end

def draw_eye(pos, focus_point)
	circle pos.x + 1, pos.y + 1, Window.width / 4 - 1, 7

	eye_angle = Math.atan2(focus_point.y - (pos.y + Window.width / 4) , focus_point.x - (pos.x + Window.width / 4))
	eye_distance = [Window.width / 8, Math.sqrt((focus_point.x - (pos.x + Window.width / 4)).pow(2) + (focus_point.y - (pos.y + Window.width / 4)).pow(2))].min
	eye_x, eye_y = (Math.cos(eye_angle) + 1) * eye_distance, (Math.sin(eye_angle) + 1) * eye_distance

	circle pos.x + (Window.width / 8 - eye_distance) + eye_x + 1, pos.y + (Window.width / 8 - eye_distance) + eye_y + 1, Window.width / 8, 0
end