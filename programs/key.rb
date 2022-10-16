class Window
  def self.text_event(char)
    $debug = !$debug if char == 32
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
    Vector.new(self.x + other.x, self.y + other.y)
  end
end

def init
  $player_pos = Vector.new(0, 0)
end

def update
  $player_pos.x -= 1 if Window.key 71
  $player_pos.x += 1 if Window.key 72
  $player_pos.y -= 1 if Window.key 73
  $player_pos.y += 1 if Window.key 74
  clear 0
  circle $player_pos.x, $player_pos.y, 10, 2
  text 1, 1, Window.key.inspect, 7
  text 1, 7, Window.button.inspect, 7
  text 1, 13, "[#{Window.mouse_x}, #{Window.mouse_y}]", 7
end