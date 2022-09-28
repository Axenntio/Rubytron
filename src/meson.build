sources = [
	'main.cpp',
	'Desktop.cpp',
	'Window.cpp',
	'helper.cpp',
	'sprites.cpp'
]

add_global_arguments('-g3', language : 'cpp')

executable(
	'rubytron',
	sources,
	include_directories: inc,
	dependencies : [sfml, mruby]
)