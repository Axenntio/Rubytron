class Window
  def self.text_event(key)
    $snake.auto = false
    $snake.keyPress(key)
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

  def ==(other)
    return self.x == other.x && self.y == other.y
  end
end

class Snake
  attr_accessor :elements
  attr_accessor :divider
  attr_accessor :direction
  attr_accessor :speed
  attr_accessor :auto
  attr_accessor :current_score

  def initialize(x, y, length, speed=10)
    self.elements = [Vector.new(x, y)]
    length.times do |i|
      self.elements << Vector.new(x + i, y)
    end
    self.divider = 0
    self.direction = nil
    self.speed = speed
    self.auto = true
    self.current_score = 0
  end

  def keyPress(key)
    Window.reload if key.chr == 'r'
    self.direction = case key.chr
    when 'a'
      Vector.new(-1, 0)
    when 'd'
      Vector.new(1, 0)
    when 'w'
      Vector.new(0, -1)
    when 's'
      Vector.new(0, 1)
    else
      self.direction
    end
  end

  def auto_play
    head = self.elements.last
    if head.x < $apple.x
      self.direction = Vector.new(1, 0)
    end
    if head.x > $apple.x
      self.direction = Vector.new(-1, 0)
    end
    if head.y < $apple.y
      self.direction = Vector.new(0, 1)
    end
    if head.y > $apple.y
      self.direction = Vector.new(0, -1)
    end
  end

  def update
    auto_play if self.auto
    return if self.direction.nil?
    self.divider -= 1
    return if self.divider.positive?
    self.elements.shift
    next_head = self.elements.last + self.direction
    next_head.x = 0 if next_head.x >= Window.width
    next_head.x = Window.width - 1 if next_head.x < 0
    next_head.y = 0 if next_head.y >= Window.height
    next_head.y = Window.height if next_head.y < 0
    self.elements << next_head
    if $apple == self.elements.last
      new_apple
      self.elements << self.elements.last
    end
    self.divider = speed
  end

  def score(value)
    self.current_score += value
    Window.title = "Snake - #{self.current_score}"
  end
end

def new_apple(scoring: true)
  $apple = Vector.new(rand(Window.width), rand(Window.height))
  Window.min_width = $apple.x + 1
  Window.min_height = $apple.y + 1
  $snake.score(10) if scoring
end

def init
  $snake = Snake.new(5, 5, 10, 10)
  new_apple(scoring: false)
end

def update(elapsed)
  $snake.update
  clear 0
  pixel $apple.x, $apple.y, 3
  $snake.elements.each do |element|
    pixel element.x, element.y, 8
  end
end