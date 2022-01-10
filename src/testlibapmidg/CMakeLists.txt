
project( ${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "libapmidg" LANGUAGES C CXX)

set(TESTSRC "testlibapmidg.c")


message(STATUS "Use ${libapmidg}")

add_executable(testapmidg ${TESTSRC})

set_target_properties(testapmidg PROPERTIES
        OUTPUT_NAME "testapmidg"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )

include_directories( "../libapmidg/" )

set(CMAKE_EXE_LINKER_FLAGS "-lze_loader -lstdc++")

target_link_libraries(testapmidg libapmidg)

install(TARGETS testapmidg
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})

