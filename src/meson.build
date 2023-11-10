source_files = files(
  'socket.cpp',
)

lib = library('smallchat',
  source_files,
  include_directories: includes,
  cpp_args: compile_args,
  dependencies: dependencies)

dep = declare_dependency(
  include_directories: includes,
  link_with: lib,
  dependencies: dependencies)

dependencies += dep
