FONT_WIDTH = 5.freeze
FONT_HEIGH = 6.freeze

$editor = nil

class Window
  def self.resize_event(new_width, new_height)
    $editor.update_show_line(new_width, new_height)
  end

  def self.text_event(char)
    $editor.add_char(char)
  end

  def self.key_press_event(key)
    $editor.parse_key(key)
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

  def -(other)
    Vector.new(self.x - other.x, self.y - other.y)
  end

  def to_s
    "{x: #{self.x}, y: #{self.y}}"
  end
end

class Editor
  attr_accessor :cursor

  def initialize(path)
    @path = path
    @cursor = Vector.new(0, 0)
    @code_shift = Vector.new(0, 0)
    @file_content = File.open(@path, 'r') do |file|
      file.read.split("\n")
    end
    @side_bar_width = @file_content.count.to_s.length
    self.update_show_line(Window.width, Window.height)
  end

  def syntax_color(word)
    syntaxes = {
      2 => ['module', 'class', 'def', 'end', 'do', 'if', 'elsif', 'else', 'case', 'when'],
      3 => [/^[A-Z]{1}[a-z_]*$/],
      8 => [/^[A-Z_]+$/],
      # 9 => [/^[A-Za-z_]*\(/], # method when previous is . or after is (
      10 => [/^\d+$/],
      12 => [/^[@:\$][A-Za-z_]+$/, 'nil', 'self', 'new', 'attr_accessor', 'attr_reader', 'attr_writer'],
    }
    syntaxes.each do |color, syntax|
      syntax.each do |pattern|
        if (pattern.is_a?(String) && pattern == word) ||
           (pattern.is_a?(Regexp) && pattern =~ word)
          return color
        end
      end
    end
    7
  end

  def update_show_line(width, height)
    @show_line = height / FONT_HEIGH + 1
    @show_char = width / FONT_WIDTH + 1 - @side_bar_width
    Window.title = "#{@path} - #{width}x#{height}"
  end

  def showable_content_postition
    Vector.new(@side_bar_width, 0)
  end

  def showable_content
    @file_content[@code_shift.y, @show_line].map do |line|
      if line.length >= @code_shift.x
        line[@code_shift.x, @show_char]
      else
        ''
      end
    end
  end

  def content_lines
    ((@code_shift.y + 1)..([@code_shift.y + @show_line, @file_content.count].min))
  end

  def showable_cursor
    @cursor - @code_shift + self.showable_content_postition
  end

  def add_char(char)
    if char.between?(32, 126) || char == 9 # tab
      @file_content[@cursor.y].insert(@cursor.x, char.chr)
      @cursor.x += 1
    end
    self.update_scope
  end

  def parse_key(key)
    case key
    when 0 # ctrl + a
      to_start_line if Window.key.include?(37)
    when 4 # ctrl + e
      to_end_line if Window.key.include?(37)
    when 17 # ctrl + r
      Window.reload if Window.key.include?(37)
    when 18 # ctrl + s
      save if Window.key.include?(37)
    when 58 # Enter
      new_line = @file_content[@cursor.y].slice!(@cursor.x, @file_content[@cursor.y].length - @cursor.x)
      @cursor.y += 1
      @cursor.x = 0
      new_line ||= '' # Makes sure we have a string since spliting at last pos only provide `nil` value
      @file_content.insert(@cursor.y, new_line)
    when 59 # Backspace
      if @cursor.x.positive?
        @file_content[@cursor.y].slice!(@cursor.x - 1)
        @cursor.x -= 1
      elsif @cursor.y.positive?
        @cursor.x = @file_content[@cursor.y - 1].length
        @file_content[@cursor.y - 1] = @file_content[@cursor.y - 1] + @file_content[@cursor.y]
        @file_content.delete_at(@cursor.y)
        @cursor.y -= 1
      end
    when 61 # Page up
      @cursor.y -= @show_line - 2
    when 62 # Page down
      @cursor.y += @show_line - 2
    when 63 # End
      to_end_line
    when 64 # Home
      to_start_line
    when 66 # Del
      if @file_content[@cursor.y].length > @cursor.x
        @file_content[@cursor.y].slice!(@cursor.x)
      else
        @file_content[@cursor.y] = @file_content[@cursor.y] + @file_content[@cursor.y + 1]
        @file_content.delete_at(@cursor.y + 1)
      end
    when 71 # Left
      @cursor.x -= 1
    when 72 # Right
      @cursor.x += 1
    when 73 # Down
      @cursor.y -= 1
      if @cursor.x > @file_content[@cursor.y].length
        @cursor.x = @file_content[@cursor.y].length
      end
    when 74 # Up
      @cursor.y += 1 if @cursor.y < (@file_content.count - 1)
      if @cursor.x > @file_content[@cursor.y].length
        @cursor.x = @file_content[@cursor.y].length
      end
    else
      puts "Unhandled #{key}"
    end
    self.update_scope
  end

  def update_scope
    @cursor.y = 0 if @cursor.y.negative?
    @cursor.y = @file_content.count - 1 if @cursor.y >= (@file_content.count - 1)
    if @cursor.x.negative?
      @cursor.y -= 1
      @cursor.x = 0
      if @cursor.y >= 0
        @cursor.x = @file_content[@cursor.y].length
      end
    end
    if @cursor.x > @file_content[@cursor.y].length
      if @cursor.y < (@file_content.count - 1)
        @cursor.x = 0
        @cursor.y += 1
      else
        @cursor.x = @file_content[@cursor.y].length
      end
    end
    @cursor.y = 0 if @cursor.y.negative?
    @cursor.y = @file_content.count - 1 if @cursor.y >= (@file_content.count - 1)

    if @cursor.x >= (@code_shift.x + @show_char - 1)
      @code_shift.x += @cursor.x - @show_char + 2 - @code_shift.x # Maybe unoptimized
    end
    if @cursor.x <= (@code_shift.x - 1)
      @code_shift.x = @cursor.x
    end
    if @cursor.y >= (@code_shift.y + @show_line - 1)
      @code_shift.y += @cursor.y - @show_line + 2 - @code_shift.y # Maybe unoptimized
    end
    if @cursor.y <= (@code_shift.y - 1)
      @code_shift.y = @cursor.y
    end
    @side_bar_width = @file_content.count.to_s.length
  end

  def to_start_line
    @cursor.x = 0
    self.update_scope
  end

  def to_end_line
    @cursor.x = @file_content[@cursor.y].length
    self.update_scope
  end

  def save
    File.open(@path, 'w') do |file|
      file.write(@file_content.join("\n"))
    end
  end
end

def init
  raise StandardError.new("Looks like you didn't pass any file as parameter") if Window.parameters[0].nil?
  $editor = Editor.new(Window.parameters[0])
end

def update
  showable = 10
  clear 0
  editor_canvas = $editor.showable_content_postition
  rect 0, 0, editor_canvas.x * FONT_WIDTH, Window.height, 1
  y_offset = 1
  $editor.content_lines.each do |number|
    line = number.to_s
    text 1 + (editor_canvas.x - line.length) * FONT_WIDTH, y_offset, line, 13, true
    y_offset += FONT_HEIGH
  end
  y_offset = 1
  $editor.showable_content.each do |line|
    parts = line.split(/([\s.,\[\]\(\)]+)/)
    part_offset = 0
    parts.each do |part|
       color = $editor.syntax_color(part)
       text 1 + editor_canvas.x * FONT_WIDTH + part_offset, y_offset, part, color, true
       part_offset += part.length * FONT_WIDTH
    end
    y_offset += FONT_HEIGH
  end
  cursor = $editor.showable_cursor
  line cursor.x * FONT_WIDTH + 1, cursor.y * FONT_HEIGH, cursor.x * FONT_WIDTH + 1, cursor.y * FONT_HEIGH + FONT_HEIGH + 1, 8
end