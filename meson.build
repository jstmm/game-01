project('game-01',
  'cpp',
  default_options: ['cpp_std=c++23']
)

src = ['src/main.cc']
include_dir = include_directories('include')

raylib_dep = dependency('raylib')

executable('game',
  sources: src,
  include_directories: include_dir,
  dependencies: [
    raylib_dep
  ]
)
