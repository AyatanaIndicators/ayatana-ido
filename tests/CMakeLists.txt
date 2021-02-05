# gtest-menuitems

set_source_files_properties(gtest-menuitems.cpp PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
add_executable("gtest-menuitems" gtest-menuitems.cpp)
target_include_directories("gtest-menuitems" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("gtest-menuitems" PUBLIC "${CMAKE_SOURCE_DIR}/src")
add_test("gtest-menuitems" "gtest-menuitems")
target_link_libraries("gtest-menuitems" ${PROJECT_DEPS_LIBRARIES} "-L${CMAKE_BINARY_DIR}/src" -layatana-ido3-0.4 gtest_main)

# coverage

find_package(CoverageReport)
ENABLE_COVERAGE_REPORT(TARGETS "gtest-menuitems" TESTS "gtest-menuitems")
