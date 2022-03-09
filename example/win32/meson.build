example_gdi_src = [
    'win32_main.cpp',
    '../samples/samples.cpp'
]
example_gdi_inc = '../samples'

example_gdi_deps = [
    gdi_dep,
    microtex_gdi_dep
]

executable(
    'microtex-win32-example', example_gdi_src,
    include_directories: example_gdi_inc,
    link_with: [microtex_gdi_lib, microtex_lib],
    dependencies: example_gdi_deps,
    install: true
)
