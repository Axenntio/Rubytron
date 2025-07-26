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

	# sfml_libs = [
	# 	'libsfml-system.dylib',
	# 	'libsfml-window.dylib',
	# 	'libsfml-graphics.dylib',
	# 	'libsfml-audio.dylib'
	# ]

	# homebrew_sfml_lib_dir = '/opt/homebrew/opt/sfml/lib'

	# foreach f : sfml_libs
	# 	install_data(
	# 		homebrew_sfml_lib_dir + '/' + f,
	# 		install_dir: 'Rubytron.app/Contents/Frameworks',
	# 		follow_symlinks: true
	# 	)
	# endforeach
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
