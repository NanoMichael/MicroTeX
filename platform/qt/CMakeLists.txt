message(STATUS "Cross-building using Qt")

set(MICROTEX_QT_sources "graphic_qt.cpp")
if (DEFINED _BUILD_STATIC AND _BUILD_STATIC)
    add_library(microtex-qt STATIC  "${MICROTEX_QT_sources}")
else ()
    add_library(microtex-qt SHARED  "${MICROTEX_QT_sources}")
endif ()

#add_library(microtex-qt SHARED graphic_qt.cpp)

set_target_properties(
    microtex-qt PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MICROTEX_API_VERSION}
    CXX_VISIBILITY_PRESET hidden
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Widgets REQUIRED)

target_include_directories(
    microtex-qt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/microtex-qt>
)

# we need to export the library on Windows
target_compile_definitions(microtex-qt PRIVATE -DMICROTEX_LIBRARY)
target_link_libraries(
    microtex-qt PRIVATE
    microtex
    Qt${QT_VERSION_MAJOR}::Gui
)

microtex_install_target(microtex-qt)
microtex_install_headers(microtex-qt HEADERS graphic_qt.h)
