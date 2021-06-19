cmake_minimum_required(VERSION 3.16)

project(gateway-server VERSION 0.1.1)

set(APP_NAME gateway-server)

set(FILES_SRC
        "main.cpp"
        "${CMAKE_SOURCE_DIR}/src/sdk/common/log/Logger.cpp"
)

add_executable(${APP_NAME} ${FILES_SRC})

set_target_properties(${APP_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
        COMPILE_OPTIONS "-std=c++2a;-O2;-Wall;-Wextra;-Wpedantic;-fsanitize=thread"
        LINK_OPTIONS "-fsanitize=thread"
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

install(TARGETS ${APP_NAME}
        DESTINATION
        "/usr/local/bin/goodok")

if (NOT WIN32)
    set(CPACK_GENERATOR DEB)
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
    set(CPACK_PACKAGE_CONTACT morozov-kst@yandex.ru)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    include(CPack)
endif()