def init
  Desktop.spawn('sprite.rb', position: [1, 8], size: [128, 128])
  Desktop.spawn('fractal_tree.rb', position: [1, 8], size: [128, 128])
  Desktop.spawn('test.rb', position: [10, 10], size: [60, 30])
  Desktop.spawn('resize.rb', position: [60, 60], size: [10, 10])
  Desktop.spawn('palette.rb', position: [30, 8], size: [20, 20])
  Desktop.spawn('key.rb', position: [85, 30], size: [40, 40])
  Desktop.spawn('xeyes.rb', position: [100, 15], size: [40, 20])
  Desktop.spawn('snake.rb', position: [120, 50], size: [40, 40])
  Desktop.spawn('editor.rb', ['editor.rb'], position: [5, 54], size: [48, 68])
  Desktop.spawn('terminal.rb', position: [57, 80], size: [60, 42])
  Window.close
end
