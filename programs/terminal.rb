FONT_WIDTH = 5.freeze
FONT_HEIGH = 6.freeze

$terminal = nil

class Window
  def self.resize_event(new_width, new_height)
    $terminal.update_showable(new_width, new_height)
  end

  def self.text_event(char)
    $terminal.input_char(char)
  end

  def self.key_press_event(key)
    $terminal.parse_key(key)
  end

  def self.focus_event(has_focus)
    $terminal.should_blink(has_focus)
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

class Terminal
  attr_reader :monospace

  def initialize
    @random = Random.new
    Window.min_height = 7
    Window.min_width = 9
    @cursor = Vector.new(0, 0)
    @blink = true
    @blink_speed = 20
    @blink_time = @blink_speed
    @sound = false
    @monospace = false
    @return_value = true
    @display_history = []
    @command_history = []
    @command_history_index = 0
    @current_line = ''
    @current_line_index = 0
    @current_path = ''
    @should_blink = Window.focused
    bin_programs = Dir.entries('bin').to_h do |file|
      next [nil, nil] unless file.end_with?(".rb")
      [file.chomp('.rb'), "bin/#{file}"]
    end.compact
    bin_programs.each do |method, file|
      Object.define_method(method) do |*args|
        execute_file file, { parameters: args, current_path: @current_path }
      end
    end
    update_showable(Window.width, Window.height)
  end

  def update_showable(width, height)
    @showable = Vector.new(width / FONT_WIDTH + 1, height / FONT_HEIGH + 1)
    Window.title = "Terminal - #{@showable.x}x#{@showable.y}"
  end

  def should_blink(should)
    @should_blink = should
    @blink = true
  end

  def showable_display(elapsed)
    @blink_time -= 1 * elapsed if @should_blink
    if @blink_time.zero? || @blink_time.negative?
      @blink_time += @blink_speed
      @blink = !@blink
    end
    line_with_index = @current_line.dup
    line_with_index[@current_line_index] = '_' if @blink
    (@display_history + [">#{line_with_index}"])[[0, @display_history.count - @showable.y + 2].max, @showable.y]
  end

  def input_char(char)
    if char.between?(32, 126)
      @current_line.insert(@current_line_index, char.chr)
      @current_line_index += 1
      sound @random.rand(220) + 440, 0.1 if @sound
    else
      sound @random.rand(110) + 220, 0.1 if @sound
    end
  end

  def parse_key(key)
    case key
    when 3 # d
      Window.close if Window.key.include?(37)
    when 17 # r
      Window.reload if Window.key.include?(37)
    when 58 # Enter
      execute
    when 59 # Backspace
      return if @current_line_index <= 0
      @current_line.slice!(@current_line_index - 1)
      @current_line_index -= 1
    when 60 # Tab
      # Pre-parse to auto-complete
    when 71 # Left
      return if @current_line_index <= 0
      @current_line_index = (@current_line_index - 1).clamp(0, @current_line.length - 1);
    when 72 # Right
      return if @current_line_index >= (@current_line.size)
      @current_line_index = (@current_line_index + 1).clamp(0, @current_line.length);
    when 73 # Up
      return if @command_history_index <= 0
      @backup_current_line = @current_line if @command_history_index == @command_history.size
      @command_history_index = (@command_history_index - 1).clamp(0, @command_history.size - 1)
      @current_line = @command_history[@command_history_index].dup || ''
      @current_line_index = @current_line.length
    when 74 # Down
      return if @command_history_index >= @command_history.size
      @command_history_index = (@command_history_index + 1).clamp(0, @command_history.size)
      @current_line =
        if @command_history_index == @command_history.size
          @backup_current_line
        else
          @command_history[@command_history_index].dup
        end
      @current_line_index = @current_line.length
    end
  end

  def execute
    _ = @_
    @command_history << @current_line
    @command_history_index = @command_history.size
    @display_history << ">#{@current_line}"
    # Parse command here
    command = @current_line.split(' ')
    result =
      case command[0]
      when 'sound'
        @sound = toggle_option(@sound, command[1])
      when 'mnsp'
        @monospace = toggle_option(@monospace, command[1])
      when 'retval'
        @return_value = toggle_option(@return_value, command[1])
      when 'autoeval'
        @auto_eval = toggle_option(@auto_eval, command[1])
      when 'history'
        history = @command_history
        history = @command_history[(@command_history.count - command[1].to_i)..@command_history.count] unless command[1].to_i.zero?
        history.each_with_index { |command, line| @display_history << "#{line + 1} #{command}" }
        nil
      when 'cd'
        if command[1].nil?
          @current_path = ''
        else
          @current_path = [@current_path, command[1]].reject(&:empty?).join('/')
          paths = @current_path.split('/')
          # Do magic with '..' and '.'
          cleaned_path = []

          paths.each do |item|
            if item == '.'
            elsif item == '..'
              cleaned_path.pop unless cleaned_path.last == '..' || cleaned_path.empty?
            else
              cleaned_path << item
            end
          end

          @current_path = cleaned_path.join('/')

          result = @current_path
        end
        true
      when 'pwd'
        result = @current_path
        @display_history << result
        result
      when 'ps'
        result = Desktop.processes
        pid_max_digits = result.max_by { |k, v| k.to_s.length }[0].to_s.length
        result.each { |pid, process| @display_history << "#{sprintf("%#{pid_max_digits}d", pid.to_i)} #{process}" }
        result
      when 'exit'
        Window.close
      when 'eval'
        eval(command.drop(1).join(' '))
      else
        begin
          eval(command.join(' '))
        rescue StandardError => e
          e
        end
      end
    @_ = result
    @display_history << result.inspect if @return_value
    @current_line = ''
    @current_line_index = 0
    @cursor.y += 1
  end

  def param_is_bool(param)
    return false if param.nil?
    param = param.downcase
    if ['0', 'false', '1', 'true'].include?(param)
      return true
    end
    false
  end

  def param_to_bool(param)
    param = param.downcase
    if ['1', 'true'].include?(param)
      return true
    elsif ['0', 'false'].include?(param)
      return false
    end
    nil
  end

  def toggle_option(option, value=nil)
    option = param_to_bool(value) if param_is_bool(value)
    option = !option if value.nil?
    option
  end
end

def init
  $terminal = Terminal.new
end

def update(elapsed)
  clear 5
  y_offset = 0;
  $terminal.showable_display(elapsed).each do |line|
    text 1, 1 + y_offset, line, 7, $terminal.monospace
    y_offset += FONT_HEIGH
  end
end