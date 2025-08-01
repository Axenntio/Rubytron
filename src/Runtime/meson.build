sources = [
	'main.cpp',
	'Player.cpp',
	shared_src
]

if host_machine.system() == 'windows'
	runtime_exe = executable(
		'runtime',
		sources,
		include_directories: inc,
		dependencies: [dep_sfml, dep_mruby],
		link_args: link_args_fix,
		win_res: '../../data/Runtime/runtime.rc'
	)
else
	runtime_exe = executable(
		'runtime',
		sources,
		include_directories: inc,
		dependencies: [dep_sfml, dep_mruby],
		link_args: link_args_fix
	)
endif

app_bundle = custom_target('RuntimeAppBundle',
	input: [runtime_exe],
	output: 'runtime-' + host_machine.system() + '.zip',
	command: [
		find_program('meson.build.' + host_machine.system() + '.sh'),
		'@INPUT0@',
		'@OUTPUT@'
	]
)