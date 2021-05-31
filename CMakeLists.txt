cmake_minimum_required(VERSION 3.16)

project(goodok-front-server)

set(APP_NAME front-server)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

conan_basic_setup()

include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-module.cmake)
include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-generate.cmake)
find_library(PROTOCOL NAMES protocol_lib HINTS "/usr/local/lib/goodok")

include_directories("${CMAKE_SOURCE_DIR}/src")

include_directories(SYSTEM
        ${CONAN_INCLUDE_DIRS_BOOST}
)

file(GLOB FILES_SRC
        "src/*.cpp"
        "src/tools/log/*.cpp"
        "src/sdk/context/*.cpp"
)

add_executable(${APP_NAME} ${FILES_SRC})
unset(FILES_SRC)

#add_custom_command(TARGET ${APP_NAME} PRE_BUILD COMMAND mkdir -p ${CMAKE_BINARY_DIR}/proto)

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
)

target_include_directories(${APP_NAME}
        PRIVATE
            "/usr/local/include/goodok"
)
target_link_libraries(${APP_NAME}
        ${PROTOCOL}
        ${CONAN_LIBS}
)

include(CTest)
enable_testing()
add_subdirectory(test)