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
	$debug = false
	$player_pos = Vector.new(0, 0)
end

def update
	puts key.inspect if $debug
	$player_pos.x -= 1 if key 71
	$player_pos.x += 1 if key 72
	$player_pos.y -= 1 if key 73
	$player_pos.y += 1 if key 74
	clear 0
	circle $player_pos.x, $player_pos.y, 10, 2
end