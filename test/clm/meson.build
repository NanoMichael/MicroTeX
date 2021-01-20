project(
    'clatexmath', 'cpp',
    default_options: ['cpp_std=c++17']
)

src = [
    '../../src/otf/clm.cpp',
    '../../src/otf/glyph.cpp',
    '../../src/otf/otf.cpp',
    '../../src/utils/utils.cpp',
    'test_clm.cpp'
]

include = include_directories('../../src')

executable(
    'clm',
    src,
    include_directories: include
)
