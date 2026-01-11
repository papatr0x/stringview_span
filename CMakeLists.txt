cmake_minimum_required(VERSION 3.28)
project(stringview_span LANGUAGES CXX)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modules are supported by Ninja generator
if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    message(WARNING "Modules are best supported by the Ninja generator.")
endif()

# string_view_examples module library
add_library(string_view_examples)
target_sources(string_view_examples
  PUBLIC
    FILE_SET CXX_MODULES FILES
    src/string_view_examples.cppm
)

# span_examples module library
add_library(span_examples)
target_sources(span_examples
  PUBLIC
    FILE_SET CXX_MODULES FILES
    src/span_examples.cppm
)

# Main executable
add_executable(stringview_span src/main.cpp)

target_link_libraries(stringview_span
  PRIVATE
    string_view_examples
    span_examples
)

# macOS specific SDK handling
if(APPLE)
    execute_process(
            COMMAND xcrun --show-sdk-path
            OUTPUT_VARIABLE MACOS_SDK_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
endif()

# Enable warnings
if(MSVC)
    target_compile_options(stringview_span PRIVATE /W4)
else()
    target_compile_options(stringview_span PRIVATE -Wall -Wextra -Wpedantic)
endif()