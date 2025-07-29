sources = [
	'main.cpp',
	'Player.cpp',
	shared_src
]

if host_machine.system() == 'darwin'
	post_install_script = 'meson.post_install.sh'
	app_dir = 'Runtime.app/Contents/MacOS'
	app_rpath = '@executable_path/../Frameworks'

	install_data(
		'../../data/Runtime/Info.plist',
		install_dir: 'Runtime.app/Contents'
	)

	install_data(
		'../../data/Runtime/icon.icns',
		install_dir: 'Runtime.app/Contents/Resources'
	)
elif host_machine.system() == 'windows'
	app_dir = 'windows'
	app_rpath = ''
else
	app_dir = 'linux'
	app_rpath = '$ORIGIN'
endif

runtime_exe = executable(
	'runtime',
	sources,
	include_directories: inc,
	dependencies: [dep_sfml, dep_mruby],
	link_args: link_args_fix,
	install: true,
	install_dir: app_dir,
	install_rpath: app_rpath
)

if is_variable('post_install_script')
	meson.add_install_script(post_install_script)
endif