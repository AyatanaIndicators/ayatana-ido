cmake_minimum_required(VERSION 3.13)
project(ayatana-ido C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

option(ENABLE_TESTS "Enable all tests and checks" OFF)
option(ENABLE_COVERAGE "Enable coverage reports (includes enabling all tests and checks)" OFF)

if(ENABLE_COVERAGE)
    set(ENABLE_TESTS ON)
    set(CMAKE_BUILD_TYPE "Coverage")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Check for prerequisites

set(DEPS glib-2.0>=2.64 gtk+-3.0>=3.24)

if(NOT DISABLE_TESTS)
    # We also need gcovr and lcov
    set(DEPS ${DEPS} gtest>=1.10)
endif()

find_package (PkgConfig REQUIRED)
pkg_check_modules(PROJECT_DEPS REQUIRED ${DEPS})

# Set global variables

include(GNUInstallDirs)
set(PROJECT_VERSION "0.8.2")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Weverything")
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wno-c++98-compat -Wno-padded")
    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wno-documentation")

else()

    set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wall")

endif()

# Make everything

add_subdirectory(src)
add_subdirectory(data)

if (NOT ENABLE_TESTS)
    message(STATUS "Unit tests disabled")
else()
    message(STATUS "Unit tests enabled")
    include(CTest)
    enable_testing()
    add_subdirectory(example)
    add_subdirectory(tests)
    ENABLE_COVERAGE_REPORT(
        TARGETS ${COVERAGE_TARGETS}
        TESTS ${COVERAGE_TEST_TARGETS}
        FILTER /usr/include ${CMAKE_BINARY_DIR}/*
    )
endif()

# Display config info

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Unit tests:: ${ENABLE_TESTS}")
