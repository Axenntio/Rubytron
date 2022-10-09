sources = [
	'main.cpp',
	'Desktop.cpp',
	'Window.cpp',
	shared_src
]

xxd = find_program('xxd')
runtime_code = custom_target('objectify-data',
	output: 'runtimes.h',
	input: runtime_exe,
	command: [ xxd, '-i', '@INPUT@', '@OUTPUT@' ],
	depends: runtime_exe
)

executable(
	'rubytron',
	[sources, runtime_code],
	include_directories: inc,
	dependencies: [sfml, mruby]
)