project(
	'Rubytron',
	'cpp',
	version: '0.1',
	meson_version: '>=1.6',
	default_options: ['warning_level=3', 'cpp_std=c++17']
)

add_global_arguments('-g3', '-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG', '-U_LIBCPP_ENABLE_ASSERTIONS', language: 'cpp')

inc = include_directories('inc')

mruby_inc = include_directories('./mruby/include/')

if host_machine.system() == 'windows'
	add_global_arguments('-DSFML_STATIC', language: 'cpp')

	link_args_fix = [
		'-L' + meson.project_source_root() / 'vcpkg' / 'installed' / 'x64-mingw-static' / 'lib',
		'-static-libgcc',
		'-static-libstdc++',
		'-static'
	]

	dep_sfml = declare_dependency(
		include_directories: include_directories('vcpkg/installed/x64-mingw-static/include'),
		link_args: [
			'-lsfml-graphics-s',
			'-lsfml-window-s',
			'-lsfml-system-s',
			'-lsfml-audio-s',
			# Graphics
			'-lfreetype',
			'-lwinmm',
			'-lgdi32',
			'-lws2_32',
			'-lopengl32',
			# Network
			'-lwsock32',
			# Audio
			'-lFLAC',
			'-lvorbis',
			'-lvorbisenc',
			'-lvorbisfile',
			'-logg'
		]
	)
	dep_mruby = declare_dependency(link_args: [files('mruby/build/host/lib/libmruby.a')[0].full_path(), '-lws2_32'], include_directories: mruby_inc)
	dep_minizip = declare_dependency(
		include_directories: include_directories('vcpkg/installed/x64-mingw-static/include/minizip-ng'),
		link_args: [
			'-lminizip-ng',
			'-lz',
			'-lbz2',
			'-llzma',
			'-lzstd',
			'-lbcrypt'
		]
	)
else
	link_args_fix = ['-L/opt/homebrew/Cellar/sfml/3.0.1/lib']
	dep_sfml = dependency('sfml-all')
	dep_mruby = declare_dependency(link_args: files('mruby/build/host/lib/libmruby.a')[0].full_path(), include_directories: mruby_inc)
	dep_minizip = dependency('minizip-ng')
endif



shared_src = [
	'../Shared/AbstractWindow.cpp',
	'../Shared/ProgramPod.cpp',
	'../Shared/helper.cpp',
	'../Shared/sprites.cpp'
]

subdir('src/Runtime')
subdir('src/Editor')