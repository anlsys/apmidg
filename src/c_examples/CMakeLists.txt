
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "C examples" LANGUAGES C CXX)

add_executable(apmidg_example_poweravg poweravg.c)
add_executable(apmidg_example_temp temp.c)
add_executable(apmidg_example_freq freq.c)
add_executable(apmidg_example_ctrlfreq ctrlfreq.c)

set_target_properties(apmidg_example_poweravg PROPERTIES
        OUTPUT_NAME "apmidg_example_poweravg"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )
set_target_properties(apmidg_example_temp PROPERTIES
        OUTPUT_NAME "apmidg_example_temp"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )
set_target_properties(apmidg_example_freq PROPERTIES
        OUTPUT_NAME "apmidg_example_freq"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )
set_target_properties(apmidg_example_ctrlfreq PROPERTIES
        OUTPUT_NAME "apmidg_example_ctrlfreq"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )


include_directories( "../libapmidg/" )

set(CMAKE_EXE_LINKER_FLAGS "-lze_loader -lstdc++")

target_link_libraries(apmidg_example_poweravg apmidg)
target_link_libraries(apmidg_example_temp apmidg)
target_link_libraries(apmidg_example_freq apmidg)
target_link_libraries(apmidg_example_ctrlfreq apmidg)

install(TARGETS apmidg_example_poweravg
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS apmidg_example_temp
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS apmidg_example_freq
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS apmidg_example_ctrlfreq
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
