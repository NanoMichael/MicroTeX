if (WASM)
    add_library(tinytex STATIC "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
else ()
    add_library(tinytex SHARED "")
endif ()

if (MSVC)
    target_compile_features(tinytex PUBLIC cxx_std_17)
    target_compile_definitions(tinytex PRIVATE -DTINYTEX_LIBRARY)
    target_compile_definitions(tinytex PUBLIC -D_HAS_STD_BYTE=1)
endif ()

set_target_properties(
    tinytex PROPERTIES
    VERSION ${VERSION_NAME}
    SOVERSION ${VERSION_NUMBER}
    CXX_VISIBILITY_PRESET hidden
)

# source files
target_sources(
    tinytex PRIVATE
    # atom folder
    atom/atom_basic.cpp
    atom/atom_char.cpp
    atom/atom_misc.cpp
    atom/atom_matrix.cpp
    atom/atom_row.cpp
    atom/atom_space.cpp
    atom/atom_stack.cpp
    atom/atom_accent.cpp
    atom/atom_scripts.cpp
    atom/atom_vrow.cpp
    atom/atom_operator.cpp
    atom/atom_zstack.cpp
    atom/atom_sideset.cpp
    atom/atom_font.cpp
    atom/atom_delim.cpp
    atom/atom_root.cpp
    atom/atom_frac.cpp
    atom/atom_fence.cpp
    atom/atom_box.cpp
    atom/atom_text.cpp
    atom/colors_def.cpp
    # box folder
    box/box.cpp
    box/box_factory.cpp
    box/box_group.cpp
    box/box_single.cpp
    # core folder
    core/split.cpp
    core/formula.cpp
    core/formula_def.cpp
    core/glue.cpp
    core/localized_num.cpp
    core/parser.cpp
    core/debug_config.cpp
    # macro folder
    macro/macro.cpp
    macro/macro_def.cpp
    macro/macro_misc.cpp
    macro/macro_scripts.cpp
    macro/macro_accent.cpp
    macro/macro_colors.cpp
    macro/macro_space.cpp
    macro/macro_delims.cpp
    macro/macro_frac.cpp
    macro/macro_styles.cpp
    macro/macro_fonts.cpp
    # env folder
    env/env.cpp
    env/units.cpp
    # utils folder
    utils/string_utils.cpp
    utils/utf.cpp
    utils/utils.cpp
    # otf folder
    otf/clm.cpp
	otf/fontsense.cpp
    otf/glyph.cpp
    otf/otf.cpp
    otf/path.cpp
    # unimath folder
    unimath/font_src.cpp
    unimath/math_type.cpp
    unimath/uni_char.cpp
    unimath/uni_font.cpp
    unimath/uni_symbol.cpp
    # graphic folder
    graphic/font_style.cpp
    graphic/graphic_basic.cpp
    graphic/graphic.cpp
    # render folder
    render/render.cpp
    render/builder.cpp

    tinytex.cpp
)
target_include_directories(tinytex PUBLIC .)

# compile option: if debug graphics
option(GRAPHICS_DEBUG "If enable graphics debug" ON)
if (GRAPHICS_DEBUG)
    add_definitions(-DGRAPHICS_DEBUG)
endif ()

# compile option: the glyph render type
# 0: use path and typeface both
# 1: use path only
# 2: use typeface only
if (WASM)
    add_definitions(-DGLYPH_RENDER_TYPE=1)
else ()
    add_definitions(-DGLYPH_RENDER_TYPE=0)
endif ()

set(
    TINYTEX_PUBLIC_HEADERS
    config.h
    version.h
    tinytex.h
    render/render.h
    utils/types.h
    unimath/font_src.h
    # graphic
    graphic/font_style.h
    graphic/graphic_basic.h
    graphic/graphic.h
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    TinyTeXConfigVersion.cmake
    VERSION ${VERSION_NAME}
    COMPATIBILITY SameMajorVersion
)

install(
    TARGETS tinytex
    EXPORT TinyTeXTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
