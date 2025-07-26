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
	app_dir = 'Rubytron.app/Contents/MacOS'
	app_rpath = '@executable_path/../Frameworks'

	install_data(
		'../../data/Info.plist',
		install_dir: 'Rubytron.app/Contents'
	)

	# install_data('data/Rubytron.icns',
	# 	install_dir: 'Rubytron.app/Contents/Resources'
	# )

	sfml_frameworks = [
		'sfml-system.framework',
		'sfml-window.framework',
		'sfml-graphics.framework',
		'sfml-audio.framework'
	]

	homebrew_sfml_framework_dir = '/opt/homebrew/opt/sfml/Frameworks'

	install_emptydir('Rubytron.app/Contents/Frameworks')
	foreach f : sfml_frameworks
		install_subdir(
			homebrew_sfml_framework_dir + '/' + f,
			install_dir: 'Rubytron.app/Contents/Frameworks'
		)
	endforeach
else
	app_dir = 'linux'
	app_rpath = '$ORIGIN'
endif

install_subdir(
	'../../programs',
	install_dir: app_dir
)

# MacOS
executable(
	'rubytron',
	[sources, runtime_code],
	include_directories: inc,
	dependencies: [dep_sfml, mruby],
	link_args: ['-L' + sfml_libdir_fix],
	install: true,
	install_dir: app_dir,
	install_rpath: app_rpath
)
