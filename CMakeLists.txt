cmake_minimum_required(VERSION 3.16)

project(goodok-front-server)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-module.cmake)
include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-generate.cmake)
find_library(PROTOCOL NAMES protocol_lib HINTS "/usr/local/lib/goodok")

add_executable(main src/main.cpp)

set_target_properties(main PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        #        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_include_directories(main
        PRIVATE
        "/usr/local/include/goodok"
)
target_link_libraries(main
        ${PROTOCOL}
        ${CONAN_LIBS}
)