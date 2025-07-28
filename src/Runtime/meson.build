sources = [
	'main.cpp',
	'Player.cpp',
	shared_src
]

runtime_exe = executable(
	'runtime-' + host_machine.system(),
	sources,
	include_directories: inc,
	dependencies: [dep_sfml, dep_mruby],
	link_args: link_args_fix
)
