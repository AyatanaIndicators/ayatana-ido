if (FLAVOUR_GTK4)
    set (FLAVOUR_GTK3 OFF)
    set (ayatana_ido_gtkver "ayatana-ido4")
elseif (FLAVOUR_GTK3)
    set (FLAVOUR_GTK4 OFF)
    set (ayatana_ido_gtkver "ayatana-ido3")
endif()

find_package(GMock)

# gtest-menuitems

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${PROJECT_DEPS_INCLUDE_DIRS}
)

set_source_files_properties(gtest-menuitems.cpp PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})

add_executable("gtest-menuitems" gtest-menuitems.cpp)
target_link_options("gtest-menuitems" PRIVATE -no-pie)
target_link_libraries("gtest-menuitems"
    ${ayatana_ido_gtkver}-0.4

    ${PROJECT_DEPS_LIBRARIES}

    ${GTEST_LIBRARIES}
    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_LIBRARIES}
)
add_test("gtest-menuitems" "gtest-menuitems")
add_dependencies("gtest-menuitems" ${ayatana_ido_gtkver}-0.4)
