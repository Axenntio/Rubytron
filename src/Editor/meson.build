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

if host_machine.system() == 'darwin'
	post_install_script = 'meson.post_install.sh'
	app_dir = 'Rubytron.app/Contents/MacOS'
	app_rpath = '@executable_path/../Frameworks'

	install_data(
		'../../data/Editor/Info.plist',
		install_dir: 'Rubytron.app/Contents'
	)

	install_data(
		'../../data/Editor/icon.icns',
		install_dir: 'Rubytron.app/Contents/Resources'
	)
elif host_machine.system() == 'windows'
	app_dir = 'windows'
	app_rpath = ''
else
	app_dir = 'linux'
	app_rpath = '$ORIGIN'
endif

install_subdir(
	'../../programs',
	install_dir: app_dir
)

executable(
	'rubytron',
	[sources, runtime_code],
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