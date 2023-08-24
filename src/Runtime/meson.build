sources = [
	'main.cpp',
	'Player.cpp',
	shared_src
]

runtime_exe = executable(
	'runtime-' + host_machine.system(),
	sources,
	include_directories: inc,
	dependencies: [dep_sfml_graphics, dep_sfml_audio, mruby]
)