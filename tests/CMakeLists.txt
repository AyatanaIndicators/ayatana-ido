find_package(GTest REQUIRED)

# gtest-menuitems

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${PROJECT_DEPS_INCLUDE_DIRS}
)

add_executable("gtest-menuitems" gtest-menuitems.cpp)
target_link_options("gtest-menuitems" PRIVATE -no-pie)
target_link_libraries("gtest-menuitems"
    ayatana-ido3-0.4

    ${PROJECT_DEPS_LIBRARIES}

    ${GTEST_LIBRARIES}
    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_LIBRARIES}
)
add_test("gtest-menuitems" "gtest-menuitems")
add_dependencies("gtest-menuitems" ayatana-ido3-0.4)
