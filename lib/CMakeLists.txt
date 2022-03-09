if (WASM)
    add_library(microtex STATIC "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
else ()
    add_library(microtex SHARED "")
endif ()

if (MSVC)
    target_compile_features(microtex PUBLIC cxx_std_17)
    target_compile_definitions(microtex PRIVATE -DMICROTEX_LIBRARY)
    target_compile_definitions(microtex PUBLIC -D_HAS_STD_BYTE=1)
endif ()

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_HOST_SYSTEM_VERSION VERSION_LESS 19)
    # std::filesystem is not available under macOS 10.15
    message(STATUS "Running on Mojave or lower, linking with Boost::filesystem")
    find_package(Boost REQUIRED COMPONENTS filesystem)
    target_link_libraries(microtex PRIVATE Boost::filesystem)
    target_compile_definitions(microtex PRIVATE USE_BOOST_FILESYSTEM)
endif ()

set_target_properties(
    microtex PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MICROTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

# source files
target_sources(
    microtex PRIVATE
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

    microtex.cpp
)
target_include_directories(
    microtex PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/microtex>
)

###################
# compile options #
###################

# compile option: if debug graphics
option(GRAPHICS_DEBUG "If enable graphics debug" ON)
if (GRAPHICS_DEBUG)
    target_compile_definitions(microtex PRIVATE -DGRAPHICS_DEBUG)
endif ()

set(_HAVE_AUTO_FONT_FIND ON)
set(_GLYPH_RENDER_TYPE 0)

if (WASM)
    set(_HAVE_AUTO_FONT_FIND OFF)
    set(_GLYPH_RENDER_TYPE 1)
endif ()

# compile option: HAVE_AUTO_FONT_FIND if enable auto font find
# this option is exported to users, will write into file 'microtexconfig.h'
option(HAVE_AUTO_FONT_FIND "If enable auto font find" ${_HAVE_AUTO_FONT_FIND})

# compile option: GLYPH_RENDER_TYPE the glyph render type
# 0: use path and typeface both
# 1: use path only
# 2: use typeface only
target_compile_definitions(microtex PRIVATE -DGLYPH_RENDER_TYPE=${_GLYPH_RENDER_TYPE})

configure_file(microtexconfig.h.in microtexconfig.h @ONLY)
target_include_directories(microtex PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

################
# installation #
################
microtex_install_target(microtex)
microtex_install_headers(
    microtex
    HEADERS ${CMAKE_CURRENT_BINARY_DIR}/microtexconfig.h microtexexport.h microtex.h microtexapi.h
)
microtex_install_headers(microtex PREFIX utils HEADERS utils/types.h)
microtex_install_headers(microtex PREFIX render HEADERS render/render.h)
microtex_install_headers(microtex PREFIX unimath HEADERS unimath/font_src.h unimath/font_meta.h)
microtex_install_headers(
    microtex
    PREFIX graphic
    HEADERS graphic/graphic.h graphic/font_style.h graphic/graphic_basic.h
)
