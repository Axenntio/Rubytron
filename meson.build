project(
	'Rubytron',
	'cpp',
	version: '0.1',
	meson_version: '>=1.6',
	default_options: ['warning_level=3', 'cpp_std=c++17']
)

add_global_arguments('-g3', '-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG', '-U_LIBCPP_ENABLE_ASSERTIONS', language: 'cpp')

inc = include_directories('inc')

sfml_libdir_fix = '/opt/homebrew/Cellar/sfml/3.0.1/lib'
dep_sfml = dependency('sfml-all')

mruby_inc = include_directories('./mruby/include/')
mruby = declare_dependency(link_args: files('mruby/build/host/lib/libmruby.a')[0].full_path(), include_directories: mruby_inc)

shared_src = [
	'../Shared/AbstractWindow.cpp',
	'../Shared/helper.cpp',
	'../Shared/sprites.cpp'
]

subdir('src/Runtime')
subdir('src/Editor')