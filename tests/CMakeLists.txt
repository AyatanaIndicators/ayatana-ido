find_package(GMock)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -no-pie")

# gtest-menuitems

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${PROJECT_DEPS_INCLUDE_DIRS}
)

set_source_files_properties(gtest-menuitems.cpp PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})

add_executable("gtest-menuitems" gtest-menuitems.cpp)
target_link_libraries("gtest-menuitems"
    ayatana-ido3-0.4

    ${PROJECT_DEPS_LIBRARIES}

    ${GTEST_LIBRARIES}
    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_LIBRARIES}
)
add_test("gtest-menuitems" "gtest-menuitems")


# coverage
if (ENABLE_COVERAGE)
    find_package(CoverageReport)
    ENABLE_COVERAGE_REPORT(TARGETS "gtest-menuitems" TESTS "gtest-menuitems")
endif()
