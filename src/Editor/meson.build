sources = [
	'main.cpp',
	'Desktop.cpp',
	'Window.cpp',
	shared_src
]

xxd = find_program('xxd')
runtime_code = custom_target('objectify-data',
	output: 'runtime-' + host_machine.system() + '.h',
	input: runtime_exe,
	command: [xxd, '-i', '@INPUT@', '@OUTPUT@'],
	depends: runtime_exe
)

executable(
	'rubytron',
	[sources, runtime_code],
	include_directories: inc,
	dependencies: [dep_sfml, mruby],
	link_args: ['-L' + sfml_libdir_fix]
)