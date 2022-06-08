
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "C examples" LANGUAGES C CXX)

add_executable(apmidg_example_poweravg poweravg.c)

set_target_properties(apmidg_example_poweravg PROPERTIES
        OUTPUT_NAME "apmidg_example_poweravg"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )

include_directories( "../libapmidg/" )

set(CMAKE_EXE_LINKER_FLAGS "-lze_loader -lstdc++")

target_link_libraries(apmidg_example_poweravg apmidg)

install(TARGETS apmidg_example_poweravg
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
