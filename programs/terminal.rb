FONT_WIDTH = 5.freeze
FONT_HEIGH = 6.freeze

$terminal = nil

class Window
	def self.resize_event(new_width, new_height)
		$terminal.update_showable(new_width, new_height)
	end

	def self.text_event(char)
        case char
		when 13
			$terminal.execute
        when 18 # ctrl + r
            reload
			$terminal.should_blink(true)
		else
			$terminal.input_char(char)
        end
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
		@display_history = []
		@command_history = []
		@current_line = ''
		@should_blink = false
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
		elsif char == 8 # Backspace
			@current_line.slice!(-1)
		elsif char == 9 # Tab
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
		when 'spawn'
			result = Window.spawn command[1], command.drop(2)
		end
		@display_history << result.to_s
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