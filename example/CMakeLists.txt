# menus

set_source_files_properties(menus.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("menus" menus.c)
target_include_directories("menus" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_link_libraries("menus" ${PROJECT_DEPS_LIBRARIES} "-L${CMAKE_BINARY_DIR}/src" -layatana-ido3-0.4)
target_include_directories("menus" PUBLIC "${CMAKE_SOURCE_DIR}/src")
