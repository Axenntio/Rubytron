class Window
  def self.resize_event(new_width, new_height)
    redraw(new_width)
  end
end

def redraw(new_width)
  scale = (new_width - 2) / 16
  Window.height = 2 + scale
  clear 0
  (0..15).each do |i|
    rect i * scale + 1, 1, scale, Window.height - 2, i
  end
end

def init
  scale = 2
  Window.min_width = 2 + 16 * scale
  Window.min_height = 2 + scale
  Window.width = Window.min_width
  redraw(Window.width)
end

def update(elapsed)
end