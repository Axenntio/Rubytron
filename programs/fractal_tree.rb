class Window
  def self.text_event(key)
    Window.reload
  end

  def self.resize_event(new_width, new_height)
    draw_fractal(new_width, new_height)
  end
end

def init
  Window.title = 'Fractree'
  draw_fractal(Window.width, Window.height)
end

def update(elapsed)
end

def draw_fractal(w, h)
  clear 0
  l = h / 3
  x = w / 2
  y = h
  a = Math::PI * 0.5

  recursion(x, y, l, a)
end

def recursion(x, y, l, a)
  dx = l * Math.cos(a)
  dy = l * Math.sin(a)
  nx = x + dx
  ny = y - dy
  line x, y, nx, ny, 7
  l *= 0.68
  da = 1.2

  recursion(nx, ny, l, a + da) if l > 1
  recursion(nx, ny, l, a - da) if l > 1
end