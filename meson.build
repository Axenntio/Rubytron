project(
	'Rubytron',
	'cpp',
	version: '0.1',
	default_options: ['warning_level=3', 'cpp_std=c++17']
)

add_global_arguments('-g3', language: 'cpp')

inc = include_directories('inc')

dep_sfml_graphics = dependency('sfml-graphics')
dep_sfml_audio = dependency('sfml-audio')

mruby_inc = include_directories('./mruby/include/')
mruby = declare_dependency(link_args: meson.source_root() + '/mruby/build/host/lib/libmruby.a', include_directories: mruby_inc)

shared_src = [
	'../Shared/AbstractWindow.cpp',
	'../Shared/helper.cpp',
	'../Shared/sprites.cpp'
]

subdir('src/Runtime')
subdir('src/Editor')