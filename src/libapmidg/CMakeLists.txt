#cmake_minimum_required(VERSION 3.9)
#set(PROJECT_NAME "libapmidg")
#set(PROJECT_VERSION 0.2.0)
project( ${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "libapmidg" LANGUAGES CXX)

set(LIBH "libapmidg.h")

#set(CMAKE_SHARED_LINK_FLAGS "-lze_loader")

file(GLOB SRCS *.cpp *.h)

add_library(apmidg SHARED ${SRCS})

set_target_properties(apmidg PROPERTIES LINK_FLAGS "-lze_loader")


set_target_properties(apmidg PROPERTIES PUBLIC_HEADER ${LIBH} RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR} VERSION ${PROJECT_VERSION})

include(GNUInstallDirs)

install(TARGETS apmidg
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
