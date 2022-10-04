class Vector
  attr_accessor :x
  attr_accessor :y

  def initialize(x, y)
    self.x = x
    self.y = y
  end

  def +(other)
    Vector.new(self.x + other.x, self.y + other.y)
  end
end

def init
  Window.title = 'xEyes'
  Window.min_width = 16
  Window.min_height = 8
end

def update
  if Window.width / 4 <= Window.height / 2
    radius = Window.width / 4
    offset = Vector.new(0, Window.height / 2 - radius)
  else
    radius = Window.height / 2
    offset = Vector.new(Window.width / 2 - radius * 2, 0)
  end
  clear 15
  mouse_pos = Vector.new(Window.mouse_x, Window.mouse_y);
  draw_eye(Vector.new(0, 0) + offset, radius, mouse_pos)
  draw_eye(Vector.new(radius * 2, 0) + offset, radius, mouse_pos)
end

def draw_eye(pos, radius, focus_point)
  circle pos.x + 1, pos.y + 1, radius - 1, 7

  eye_angle = Math.atan2(focus_point.y - (pos.y + radius) , focus_point.x - (pos.x + radius))
  eye_distance = [radius / 2, Math.sqrt((focus_point.x - (pos.x + radius)).pow(2) + (focus_point.y - (pos.y + radius)).pow(2))].min
  eye_x, eye_y = (Math.cos(eye_angle) + 1) * eye_distance, (Math.sin(eye_angle) + 1) * eye_distance

  circle pos.x + (radius / 2 - eye_distance) + eye_x + 1, pos.y + (radius / 2 - eye_distance) + eye_y + 1, radius / 2, 0
end