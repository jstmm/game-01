project('game-01', 'cpp',
        default_options: ['cpp_std=c++23'])

incdir = include_directories('include')

raylib = subproject('raylib',
    git: 'https://github.com/raysan5/raylib.git',
    revision: '5.5'
)
raylib_dep = raylib.get_dependency()

executable('game', 'src/main.cc',
          include_directories : incdi,
          dependencies: [raylib_dep])
