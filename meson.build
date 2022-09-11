project(
	'Rubytron',
	'cpp',
	version : '0.1',
	default_options : ['warning_level=3', 'cpp_std=c++17']
)

inc = include_directories('inc')

sfml = dependency('sfml-graphics')

mruby_inc = include_directories('./mruby/include/')
mruby = declare_dependency(link_args: meson.source_root() + '/mruby/build/host/lib/libmruby.a', include_directories: mruby_inc)

subdir('src')