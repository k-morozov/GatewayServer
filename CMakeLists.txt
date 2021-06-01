cmake_minimum_required(VERSION 3.16)

project(goodok-servers)

set(APP_NAME goodok-servers)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-module.cmake)
include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-generate.cmake)

find_library(PROTOCOL NAMES protocol_lib HINTS "/usr/local/lib/goodok")

include_directories(SYSTEM
        ${CONAN_INCLUDE_DIRS_BOOST}
)
include_directories("${CMAKE_SOURCE_DIR}/src")

add_subdirectory("src/sdk")
add_subdirectory("src/app/gateway")

include(CTest)
enable_testing()
add_subdirectory(test)