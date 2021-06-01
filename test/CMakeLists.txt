cmake_minimum_required(VERSION 3.10)

set(APP_NAME ut_server)
#set(CONAN_DISABLE_CHECK_COMPILER TRUE)

file(GLOB SRC_FILES
        "${CMAKE_SOURCE_DIR}/src/tools/log/*.cpp"
        "*.cpp"
)

add_executable(${APP_NAME} ${SRC_FILES})
include_directories(${CONAN_INCLUDE_DIRS_GTEST})

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
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

add_test(NAME ${APP_NAME} COMMAND $<TARGET_FILE:${APP_NAME}>)