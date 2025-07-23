parameters = ARGV[0][:parameters]
current_path = ARGV[0][:current_path]

Desktop.spawn "#{current_path}/#{parameters[1]}", parameters.drop(2)
