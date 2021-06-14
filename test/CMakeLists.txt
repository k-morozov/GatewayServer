cmake_minimum_required(VERSION 3.10)

set(APP_NAME ut_server)

file(GLOB SRC_FILES
        "${CMAKE_SOURCE_DIR}/src/sdk/log/*.cpp"
        "*.cpp"
)

add_executable(${APP_NAME} ${SRC_FILES})
include_directories(${CONAN_INCLUDE_DIRS_GTEST})

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++2a;-O2;-Wall;-Wextra;-Wpedantic;-fsanitize=thread"
        LINK_OPTIONS "-fsanitize=thread"
        )

target_link_libraries(${APP_NAME}
        ${PROTOCOL}
        ${CONAN_LIBS_GTEST}
        ${CONAN_LIBS}
        sdk-goodok
)

target_include_directories(${APP_NAME}
        PRIVATE
        "/usr/local/include/goodok"
)

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)

add_test(NAME ${APP_NAME} COMMAND $<TARGET_FILE:${APP_NAME}>)