cmake_minimum_required(VERSION 3.9)

set(PROJECT_NAME "libapmidg")

set(PROJECT_VERSION 0.4.0)

project( ${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "libapmidg" LANGUAGES CXX)

set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin/${PLATFORM_DIR}/${BUILD_TYPE_DIR}")

add_subdirectory("src/libapmidg")
add_subdirectory("src/tools")
add_subdirectory("src/pyapmidg")
add_subdirectory("src/c_examples")


