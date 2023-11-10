project('smallchat', 'cpp',
  version: '0.0.1',
  default_options: [
    'buildtype=release',
    'default_library=static',
    'strip=true',
    'cpp_std=c++20',
    'warning_level=3'],
  meson_version: '>=1.0.0')

includes = include_directories('include')

compile_args = []

fmt_dep = dependency('fmt')

dependencies = []
dependencies += fmt_dep

subdir('src')
subdir('tests')
