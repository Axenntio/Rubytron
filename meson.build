project(
	'Rubytron',
	'cpp',
	version: '0.1',
	meson_version: '>=1.3',
	default_options: ['warning_level=3', 'cpp_std=c++17']
)

add_global_arguments('-g3', language: 'cpp')

inc = include_directories('inc')

sfml_libdir_fix = '/opt/homebrew/Cellar/sfml/3.0.1/lib'
dep_sfml = dependency('sfml-all')

mruby_inc = include_directories('./mruby/include/')
mruby = declare_dependency(link_args: meson.project_source_root() + '/mruby/build/host/lib/libmruby.a', include_directories: mruby_inc)

shared_src = [
	'../Shared/AbstractWindow.cpp',
	'../Shared/helper.cpp',
	'../Shared/sprites.cpp'
]

subdir('src/Runtime')
subdir('src/Editor')