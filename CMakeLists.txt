cmake_minimum_required(VERSION 3.16)

project(goodok-servers)

set(APP_NAME goodok-servers)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CONAN_DISABLE_CHECK_COMPILER TRUE)

conan_basic_setup()

include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-module.cmake)
include(${CONAN_LIB_DIRS_PROTOBUF}/cmake/protobuf/protobuf-generate.cmake)

find_library(PROTOCOL NAMES protocol_lib HINTS "/usr/local/lib/goodok")

include_directories(SYSTEM
        ${CONAN_INCLUDE_DIRS}
)
include_directories("${CMAKE_SOURCE_DIR}/src")

add_subdirectory("src/sdk")
add_subdirectory("src/app/gateway")

include(CTest)
enable_testing()
add_subdirectory(test)

#if (NOT WIN32)
#    set(CPACK_GENERATOR DEB)
#    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
#    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
#    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
#    set(CPACK_PACKAGE_CONTACT morozov-kst@yandex.ru)
#    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
#    include(CPack)
#endif()