cmake_minimum_required(VERSION 3.13)
project(ayatana-ido C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# Options

option(ENABLE_TESTS "Enable all tests and checks" OFF)
option(ENABLE_COVERAGE "Enable coverage reports (includes enabling all tests and checks)" OFF)
option(ENABLE_WERROR "Treat all build warnings as errors" OFF)

if(ENABLE_COVERAGE)
    set(ENABLE_TESTS ON)
    set(CMAKE_BUILD_TYPE "Coverage")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(ENABLE_COVERAGE)
    set(ENABLE_TESTS ON)
    set(CMAKE_BUILD_TYPE "Coverage")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Check for prerequisites

option(FLAVOUR_GTK3 "Build against GTK+-3.0" ON)
option(FLAVOUR_GTK4 "Build against GTK+-4.0" OFF)
option(ENABLE_LOADER "Build Ayatana Indicator Loader" ON)
option(ENABLE_IDO "Enable IDO specific code" ON)

if (FLAVOUR_GTK4)
    set (FLAVOUR_GTK3 OFF)
    set (ayatana_ido_gtkver "ayatana-ido4")
endif()
if (FLAVOUR_GTK3)
    set (FLAVOUR_GTK4 OFF)
    set (ayatana_ido_gtkver "ayatana-ido3")
endif()

set(DEPS glib-2.0>=2.58)

if (FLAVOUR_GTK4)
    set(DEPS
        ${DEPS}
        gtk+-4.0>=3.91
    )
endif()
if (FLAVOUR_GTK3)
    set(DEPS
        ${DEPS}
        gtk+-3.0>=3.24
    )
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

if (ENABLE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(example)
    add_subdirectory(tests)
    # coverage
    if (ENABLE_COVERAGE)
        find_package(CoverageReport)
        ENABLE_COVERAGE_REPORT(
            TARGETS "${ayatana_ido_gtkver}-0.4"
            TESTS "gtest-menuitems"
            FILTER /usr/include ${CMAKE_BINARY_DIR}/*
        )
    endif()
endif()

# Display config info

message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "GTK+-4.0 build: ${FLAVOUR_GTK4}")
message(STATUS "GTK+-3.0 build: ${FLAVOUR_GTK3}")
message(STATUS "Unit tests: ${ENABLE_TESTS}")
message(STATUS "Build with -Werror: ${ENABLE_WERROR}")
