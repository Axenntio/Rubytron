sources = [
	'main.cpp',
	'Desktop.cpp',
	'Window.cpp',
	'helper.cpp',
	shared_src
]

xxd = find_program('xxd')
runtime_code = custom_target('objectify-data',
	output: 'runtime-' + host_machine.system() + '.h',
	input: app_bundle,
	command: [xxd, '-i', '@INPUT@', '@OUTPUT@'],
	depends: app_bundle
)

if host_machine.system() == 'darwin'
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
	rc_obj = windows.compile_resources('../../data/Editor/editor.rc')
	sources += rc_obj
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
	dependencies: [dep_sfml, dep_mruby, dep_minizip],
	link_args: link_args_fix,
	install: true,
	install_dir: app_dir,
	install_rpath: app_rpath
)

fs = import('fs')
filepath = join_paths(meson.current_source_dir(), 'meson.post_install.' + host_machine.system() + '.sh')

if fs.exists(filepath)
	meson.add_install_script(filepath)
endif