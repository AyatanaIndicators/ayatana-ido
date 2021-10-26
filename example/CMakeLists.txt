if (FLAVOUR_GTK4)
    set (ayatana_ido_gtkver "ayatana-ido4")
endif()
if (FLAVOUR_GTK3)
    set (ayatana_ido_gtkver "ayatana-ido3")
endif()

# menus

set_source_files_properties(menus.c PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("menus" menus.c)
target_include_directories("menus" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_link_libraries("menus" ${PROJECT_DEPS_LIBRARIES} "-L${CMAKE_BINARY_DIR}/src" -l${ayatana_ido_gtkver}-0.4)
target_include_directories("menus" PUBLIC "${CMAKE_SOURCE_DIR}/src")
add_dependencies("menus" ${ayatana_ido_gtkver}-0.4)
