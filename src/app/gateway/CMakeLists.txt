cmake_minimum_required(VERSION 3.16)

project(gateway-server)

set(APP_NAME gateway-server)

set(FILES_SRC
        "main.cpp"
        "${CMAKE_SOURCE_DIR}/src/tools/log/Logger.cpp"
)

add_executable(${APP_NAME} ${FILES_SRC})

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
        sdk-goodok
)