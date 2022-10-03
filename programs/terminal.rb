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
		Window.min_height = 7
		Window.min_width = 9
		@cursor = Vector.new(0, 0)
		@blink = true
		@blink_speed = 12
		@blink_time = @blink_speed
		@monospace = false
		@return_value = false
		@display_history = []
		@command_history = []
		@current_line = ''
		@current_path = 'programs'
		@should_blink = Window.focused
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

	def showable_display
		cursor = ''
		@blink_time -= 1 if @should_blink
		if @blink_time.zero?
			@blink_time = @blink_speed
			@blink = !@blink
		end
		cursor = '_' if @blink
		(@display_history + [">#{@current_line}#{cursor}"])[[0, @display_history.count - @showable.y + 2].max, @showable.y]
	end

	def input_char(char)
		if char.between?(32, 126)
            @current_line += char.chr
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
			@current_line.slice!(-1)
		when 60 # Tab
			# Pre-parse to auto-complete
		end
	end

	def execute
		@command_history << @current_line
		@display_history << ">#{@current_line}"
		# Parse command here
		command = @current_line.split(' ')
		result = nil
		case command[0]
		when 'monospace'
			value = command[1]
			@monospace = param_to_bool(value) if param_is_bool(value) unless value.nil?
			result = @monospace
		when 'return_value'
			value = command[1]
			@return_value = param_to_bool(value) if param_is_bool(value) unless value.nil?
			result = @return_value
		when 'spawn'
			result = Desktop.spawn "#{@current_path}/#{command[1]}", command.drop(2)
		when 'history'
			history = @command_history
			history = @command_history[(@command_history.count - command[1].to_i)..@command_history.count] if command[1].to_i.is_a?(Integer) # TODO: Check working
			history.each_with_index { |command, line| @display_history << "#{line + 1} #{command}" }
		when 'cd'
			if command[1].nil?
				@current_path = 'programs'
			else
				@current_path += "/#{command[1]}"
				paths = @current_path.split('/')
				# Do magic with '..' and '.'
				result = @current_path
			end
		when 'pwd'
			result = @current_path
			@display_history << result
		when 'ps'
			result = Desktop.processes
			result.each { |process| @display_history << process }
		when 'kill'
			result = Desktop.kill_process(command[1].to_i) if command[1].to_i.is_a?(Integer)
		when 'exit'
			Window.close
		end
		@display_history << result.inspect if @return_value
		@current_line = ''
		@cursor.y += 1
	end

	def param_is_bool(param)
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
end

def init
	$terminal = Terminal.new
end

def update
	clear 5
	y_offset = 0;
	$terminal.showable_display.each do |line|
		text 1, 1 + y_offset, line, 7, $terminal.monospace
		y_offset += FONT_HEIGH
	end
end