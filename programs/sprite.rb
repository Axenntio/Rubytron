class Window
  def self.resize_event(_, _)
    $editor.redraw
  end

  def self.text_event(key)
    Window.reload
  end

  def self.button_press_event(button)
    $editor.click_event(x: Window.mouse_x, y: Window.mouse_y, button: button)
  end
end

class Interactive
  attr_accessor :x
  attr_accessor :y
  attr_accessor :width
  attr_accessor :height
  attr_accessor :scale

  def initialize(x: 0, y: 0, scale: 8)
    self.x = x
    self.y = y
    self.scale = scale
    @need_redraw = true
  end

  def in?(x:, y:)
    x > self.x && x < self.x + self.width && y > self.y && y < self.y + self.height
  end

  def redraw?
    @need_redraw
  end

  def redraw
    @need_redraw = true
  end

  def draw
    @need_redraw = false
  end
end

class PaletteSelector < Interactive
  attr_accessor :selected

  def initialize(x: 0, y: 0, scale: 8)
    super(x: x, y: y, scale: scale)
    self.width = self.scale * 4
    self.height = self.scale * 4
    self.selected = 0
  end

  def click_event(x:, y:)
    self.selected = (x - self.x) / scale + (y - self.y) / scale * 4
    redraw
  end

  def draw
    super
    rect self.x, self.y, self.scale * 4 + 1, self.scale * 4 + 1, 0
    4.times do |dx|
      4.times do |dy|
        palette = dx + dy * 4
        rect self.x + dx * self.scale, self.y + dy * self.scale, self.scale + 1, self.scale + 1, 6 if palette == self.selected
        rect self.x + dx * self.scale + 1, self.y + dy * self.scale + 1, self.scale - 1, self.scale - 1, palette
      end
    end
  end
end

class SpritePalette < Interactive
  attr_accessor :sprite_x
  attr_accessor :sprite_y
  attr_accessor :sprite_x_index
  attr_accessor :sprite_y_index
  attr_accessor :canvas

  def initialize(x: 0, y: 0, scale: 8, sprite_x: 16, sprite_y: 4, sprite_x_index: 0, sprite_y_index: 0)
    super(x: x, y: y, scale: scale)
    self.width = self.scale * sprite_x
    self.height = self.scale * sprite_y
    self.sprite_x = sprite_x
    self.sprite_y = sprite_y
    self.sprite_x_index = sprite_x_index
    self.sprite_y_index = sprite_y_index
    self.canvas = Array.new(self.sprite_x * self.sprite_y * 8 * 8) { |i| 0 }
  end

  def click_event(x:, y:)
    self.sprite_x_index = (x - self.x) / scale
    self.sprite_y_index = (y - self.y) / scale
    redraw
  end

  def draw_on(x:, y:, color: )
    self.canvas[(x + self.sprite_x_index * self.scale) + (y + (self.sprite_y_index * self.scale)) * self.scale * self.sprite_y * self.sprite_y] = color
  end

  def get_at(x:, y:)
    self.canvas[(x + self.sprite_x_index * self.scale) + (y + (self.sprite_y_index * self.scale)) * self.scale * self.sprite_y * self.sprite_y]
  end

  def draw
    super
    rect self.x - 1, self.y - 1, self.width + 2, self.height + 2, 0
    self.sprite_x.times do |dx|
      self.sprite_y.times do |dy|
        8.times do |sx|
          8.times do |sy|
            rect self.x + dx * self.scale + sx, self.y + dy * self.scale + sy, 1, 1, self.canvas[(dy * self.scale + sy) * (self.sprite_x * self.scale) + dx * self.scale + sx]
          end
        end
      end
      rect self.x + self.sprite_x_index * self.scale - 1, self.y + self.sprite_y_index * self.scale - 1, self.scale + 2, self.scale + 2, 7
      8.times do |sx|
        8.times do |sy|
          rect self.x + self.sprite_x_index * self.scale + sx, self.y + self.sprite_y_index * self.scale + sy, 1, 1, self.canvas[(self.sprite_y_index * self.scale + sy) * (self.sprite_x * self.scale) + self.sprite_x_index * self.scale + sx]
        end
      end
    end
  end
end

class DrawingCanvas < Interactive
  attr_accessor :sprite_palette
  attr_accessor :color_palette

  def initialize(x: 0, y: 0, scale: 8, sprite_palette:, color_palette:)
    super(x: x, y: y, scale: scale)
    self.width = self.scale * 8
    self.height = self.scale * 8
    self.sprite_palette = sprite_palette
    self.color_palette = color_palette
  end

  def click_event(x:, y:)
    @sprite_palette.draw_on(x: (x - self.x) / scale, y: (y - self.y) / scale, color: self.color_palette.selected)
    redraw
  end

  def draw
    super
    rect self.x - 1, self.y - 1, self.scale * 8 + 2, self.scale * 8 + 2, 0
    8.times do |dx|
      8.times do |dy|
        pixel = dx + dy * 8
        rect self.x + dx * self.scale, self.y + dy * self.scale, self.scale, self.scale, self.sprite_palette.get_at(x: dx, y: dy)
      end
    end
  end
end

class Editor
  def initialize
    @sprite_palette = SpritePalette.new
    @color_palette = PaletteSelector.new
    @canvas = DrawingCanvas.new(x: 4, y: 4, sprite_palette: @sprite_palette, color_palette: @color_palette)
    @color_palette.x = @canvas.x + @canvas.width + 8
    @color_palette.y = @canvas.y + @canvas.width / 2 - @color_palette.width / 2
    @sprite_palette.x = @canvas.x
    @sprite_palette.y = @canvas.y + @canvas.width + 3
  end

  def click_event(x:, y:, button:)
    @color_palette.click_event(x: x, y: y) if @color_palette.in?(x: x, y: y)
    @sprite_palette.click_event(x: x, y: y) if @sprite_palette.in?(x: x, y: y)
  end

  def redraw
    @canvas.redraw
    @sprite_palette.redraw
    @color_palette.redraw
  end

  def update
    x = Window.mouse_x
    y = Window.mouse_y
    @canvas.click_event(x: x, y: y) if Window.button.include?(0) && @canvas.in?(x: x, y: y)
  end

  def draw
    return unless @canvas.redraw? || @sprite_palette.redraw? || @color_palette.redraw?

    clear 1
    @canvas.draw
    @sprite_palette.draw
    @color_palette.draw
  end
end

def init
  $editor = Editor.new
end

def update(elapsed)
  $editor.update
  $editor.draw
end