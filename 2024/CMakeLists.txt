cmake_minimum_required(VERSION 3.15)

# Set CMAKE_TOOLCHAIN_FILE before call to project()
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
else()
    message(FATAL_ERROR "VCPKG_ROOT environment variable not set")
endif()

project(AdventOfCode C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Stb REQUIRED)

add_library(CommonIncludes INTERFACE)
target_include_directories(CommonIncludes INTERFACE ${Stb_INCLUDE_DIR})

# Turn on all warnings
if(MSVC)
    target_compile_options(CommonIncludes INTERFACE /W4 /WX)
else()
    target_compile_options(CommonIncludes INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

# Create targets for all source files and include the stb header
file(GLOB SRC_FILES src/day_*.c)

foreach(SRC_FILE ${SRC_FILES})
    get_filename_component(TARGET_NAME ${SRC_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${SRC_FILE})

    target_link_libraries(${TARGET_NAME} PRIVATE CommonIncludes)
endforeach()

# Include the symbol SYSTEM to suppress warnings from stb headers
include_directories(SYSTEM ${Stb_INCLUDE_DIR})
