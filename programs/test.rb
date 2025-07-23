class Window
  def self.resize_event(new_width, new_height)
    # puts "Received resize event (#{new_width}, #{new_height})"
    $should_update = true
  end

  def self.close_event
    # puts 'Received close event'
  end

  def self.text_event(char)
    # puts "Received text event (#{char})"
  end

  def self.key_press_event(key)
    # puts "Key pressed (#{key})"
  end

  def self.key_release_event(key)
    # puts "Key released (#{key})"
  end

  def self.button_press_event(button)
    # puts "Button pressed (#{button})"
  end

  def self.button_release_event(button)
    # puts "Button released (#{button})"
  end

  def self.focus_event(has_focus)
    # puts "Focus changed (#{has_focus})"
    $has_focus = has_focus
  end

  def self.mouse_wheel_event(horizontal, vertical)
    # puts "Mouse wheel scroll [#{horizontal}, #{vertical}]"
  end
end

def init
  $has_focus = false
  $should_update = true
  $color_index = 0
  $value = 0
end

def update(elapsed)
  return if $has_focus && !$should_update

  clear $color_index
  $value += elapsed / 10 if $has_focus
  pixel 0, 0, 1
  pixel 0, Window.height - 1, 2
  pixel Window.width - 1, 0, 3
  pixel Window.width - 1, Window.height - 1, 4
  pixel Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 5
  line 20, 20, Math.cos($value) * 10 + 20, Math.sin($value) * 10 + 20, 9
  pixel Math.cos($value / 2) * 4 + 20, Math.sin($value / 2) * 4 + 20, 6
  line 0, 0, 10, 10, 7
  line 10, 10, 25, 15, 8
  text 1, Window.height - 7, 'Hey! Example demo.'
end