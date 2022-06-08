
project( ${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "libapmidg" LANGUAGES C CXX)

set(TESTSRC "apmidgstats.c")

# message(STATUS "Use ${TESTSRC}")

add_executable(apmidgstats ${TESTSRC})

set_target_properties(apmidgstats PROPERTIES
        OUTPUT_NAME "apmidgstats"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )

include_directories( "../libapmidg/" )

set(CMAKE_EXE_LINKER_FLAGS "-lze_loader -lstdc++")

target_link_libraries(apmidgstats apmidg)

install(TARGETS apmidgstats
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
