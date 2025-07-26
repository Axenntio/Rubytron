$friction = 0.97
$gravity = 0.2
$bounce = 0.9

class Window
  def self.text_event(key)
    reload if key.chr == 'r'
  end

  def self.resize_event(new_width, new_height)
    $canvas.size = Vector.new(new_width, new_height)
  end

  def self.button_press_event(button)
    $canvas.on_click(Window.mouse_x, Window.mouse_y, button)
  end
end

class Vector
  attr_accessor :x
  attr_accessor :y

  def initialize(x, y)
    self.x = x
    self.y = y
  end

  def +(other)
    return Vector.new(self.x + other.x, self.y + other.y) if other.is_a?(Vector)
    return Vector.new(self.x + other, self.y + other) if [Integer, Float].include?(other.class)
    raise "Unhandled type #{other.class}"
  end

  def -(other)
    return Vector.new(self.x - other.x, self.y - other.y) if other.is_a?(Vector)
    return Vector.new(self.x - other, self.y - other) if [Integer, Float].include?(other.class)
    raise "Unhandled type #{other.class}"
  end

  def *(other)
    return Vector.new(self.x * other.x, self.y * other.y) if other.is_a?(Vector)
    return Vector.new(self.x * other, self.y * other) if [Integer, Float].include?(other.class)
    raise "Unhandled type #{other.class}"
  end

  def to_s
    "{x: #{self.x}, y: #{self.y}}"
  end

  def render
    pixel(self.x, self.y, 8)
  end
end

class Point
  attr_accessor :pos
  attr_accessor :old_pos
  attr_accessor :pinned

  def initialize(x, y, vx=0, vy=0, pinned=false)
    self.pos = Vector.new(x, y)
    self.old_pos = Vector.new(x - vx, y - vy)
    self.pinned = pinned
  end

  def distance_to(point)
    delta = self.pos - point.pos
    Math.sqrt(delta.x * delta.x + delta.y * delta.y)
  end

  def update
    if self.pinned != true then
      velocity = (self.pos - self.old_pos) * $friction

      self.old_pos = self.pos
      self.pos += velocity
      self.pos += Vector.new(0, $gravity)
    end
  end

  def constrain
    if !self.pinned then
      velocity = (self.pos - self.old_pos) * $friction

      if self.pos.x > $canvas.size.x then
        self.pos.x = $canvas.size.x
        self.old_pos.x = self.pos.x + velocity.x * $bounce
      elsif self.pos.x < 0 then
        self.pos.x = 0
        self.old_pos.x = self.pos.x + velocity.x * $bounce
      end
      if self.pos.y > $canvas.size.y then
        self.pos.y = $canvas.size.y
        self.old_pos.y = self.pos.y + velocity.y * $bounce
      elsif self.pos.y < 0 then
        self.pos.y = 0
        self.old_pos.y = self.pos.y + velocity.y * $bounce
      end
    end
  end

  def render
    # self.pos.render
  end
end

class Constrain
  attr_accessor :p1
  attr_accessor :p2
  attr_accessor :length
  attr_accessor :hidden

  def initialize(p1, p2, visible = true, length = nil)
    self.p1 = p1
    self.p2 = p2
    if length.nil?
      self.length = p1.distance_to(p2)
    else
      self.length = length
    end
    self.hidden = !visible
  end

  def update
    delta = self.p2.pos - self.p1.pos
    dist = Math.sqrt(delta.x * delta.x + delta.y * delta.y)
    diff = self.length - dist
    percent = diff / dist / 2
    offset = delta * percent

    unless self.p1.pinned then
      self.p1.pos -= offset
    end
    unless self.p2.pinned then
      self.p2.pos += offset
    end
  end

  def render
    return if self.hidden
    line(self.p1.pos.x, self.p1.pos.y, self.p2.pos.x, self.p2.pos.y, 7)
  end
end

class Canvas
  attr_accessor :points
  attr_accessor :constrains
  attr_accessor :size

  def initialize
    self.size = Vector.new(Window.width - 1, Window.height - 1)
    self.points = []
    self.constrains = []
    create_cube(4, 4, 16, 16)
    create_rope(64, 4, 160, 4, 1)
    create_rope(60, 4, 156, 4, 8)
    create_rope(68, 4, 364, 4)
  end

  def create_cube(x1, y1, x2, y2)
    p = [
      Point.new(x1, y1, 5, -5),
      Point.new(x1, y2),
      Point.new(x2, y2),
      Point.new(x2, y1)
    ]
    c = [
      Constrain.new(p[0], p[1]),
      Constrain.new(p[1], p[2]),
      Constrain.new(p[2], p[3]),
      Constrain.new(p[3], p[0]),
      Constrain.new(p[0], p[2], false)
    ]
    self.points += p
    self.constrains += c
  end

  def create_rope(x1, y1, x2, y2, segments=nil)
    p = [Point.new(x1, y1, 0, 0, true)]
    c = []

    length = p[0].distance_to(Point.new(x2, y2))
    segments = length if segments.nil?
    segment_length = length / segments
    if segments > length / 4
      segments = (length / 4).to_i
      segment_length = 4
    end

    segments.times do |i|
      p << Point.new(x1 + (i + 1) * segment_length, y1)
      c << Constrain.new(p[i], p[i + 1])
    end
    self.points += p
    self.constrains += c
  end

  def on_click(x, y, button)
    if button == 1
      self.points.last.pinned = !self.points.last.pinned
    end
  end

  def update
    if Window.button 0
      self.points.last.pos = Vector.new(Window.mouse_x, Window.mouse_y)
    end
    self.points.each(&:update)
    5.times do
      self.points.each(&:constrain)
      self.constrains.each(&:update)
    end
  end

  def render
    self.points.each(&:render)
    self.constrains.each(&:render)
  end
end

def init
  Window.title = 'Verlet'
  $canvas = Canvas.new
end

def update(elapsed)
  $canvas.update
  clear 0
  $canvas.render
end