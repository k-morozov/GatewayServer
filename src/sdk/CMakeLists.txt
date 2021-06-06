cmake_minimum_required(VERSION 3.16)

project(sdk-goodok)

set(APP_NAME sdk-goodok)

file(GLOB FILES_SRC
        "common/log/*.cpp"
        "common/*.h"
        "context/*.cpp"
        "network/*.cpp"
        "network/AcceptProcess.h"
        "network/session/*.cpp"
        "engine/*.cpp"
)

add_library(${APP_NAME} STATIC ${FILES_SRC})

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++2a;-O2;-Wall;-Wextra;-Wpedantic"
        )

target_include_directories(${APP_NAME}
        PUBLIC
            "common/"
            "context/"
            "network/"
        PRIVATE
            "/usr/local/include/goodok"
)

target_link_libraries(${APP_NAME}
        ${PROTOCOL}
        ${CONAN_LIBS}
)