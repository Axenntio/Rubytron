class Window
  def self.text_event(key)
    Window.reload
  end
end

def init
  $shift = 0
end

def update(elapsed)
  clear 2
  radius = 20
  circle Window.width / 2 - radius, Window.height / 2 - radius, radius, 0
  circle Window.width / 2 - (radius + 1) + Math.cos($shift) * 3, Window.height / 2 - (radius + 1) + Math.sin($shift) * 3, (radius + 1), 2
  moving_radius = radius / 2 + Math.cos($shift / 2) * 4
  circle Window.width / 2 - moving_radius + 1, Window.height / 2 - moving_radius + 1, moving_radius, 0
  $shift += 0.06 * elapsed
end