sources = [
	'main.cpp',
	'Player.cpp',
	shared_src
]

runtime_exe = executable(
	'runtime',
	sources,
	include_directories: inc,
	dependencies: [dep_sfml, dep_mruby],
	link_args: link_args_fix
)

if host_machine.system() == 'darwin'
	post_install_script = 'meson.post_install.sh'

	app_bundle = custom_target('RuntimeAppBundle',
		input: [runtime_exe],
		output: 'runtime-darwin.zip',
		command: [
			find_program('meson.build.darwin.sh'),
			'@INPUT0@',
			'@OUTPUT@'
		],
		install: true,
		install_dir: '.'
	)
elif host_machine.system() == 'windows'
	app_bundle = runtime_exe
endif
