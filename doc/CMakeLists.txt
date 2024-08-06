set(cwd ${CMAKE_CURRENT_LIST_DIR})
set(source_root ${microtex_SOURCE_DIR})
set(version ${microtex_VERSION})
configure_file(microtex.doxy.in microtex.doxy @ONLY)
