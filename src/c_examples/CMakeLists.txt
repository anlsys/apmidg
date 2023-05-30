
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "C examples" LANGUAGES C CXX)

add_executable(apmidg_sweep_pwrlim sweep_pwrlim.c)
add_executable(apmidg_example_poweravg poweravg.c)
add_executable(apmidg_example_temp temp.c)
add_executable(apmidg_example_freq freq.c)
add_executable(apmidg_example_ctrlfreq ctrlfreq.c)
add_executable(standalone_energy_reader standalone_energy_reader.c)

set_target_properties(apmidg_sweep_pwrlim PROPERTIES
        OUTPUT_NAME "apmidg_sweeep_pwrlim"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )
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
set_target_properties(standalone_energy_reader PROPERTIES
        OUTPUT_NAME "standalone_energy_reader"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )

include_directories( "../libapmidg/" )

set(CMAKE_EXE_LINKER_FLAGS "-lze_loader -lstdc++")

target_link_libraries(apmidg_sweep_pwrlim apmidg)
target_link_libraries(apmidg_example_poweravg apmidg)
target_link_libraries(apmidg_example_temp apmidg)
target_link_libraries(apmidg_example_freq apmidg)
target_link_libraries(apmidg_example_ctrlfreq apmidg)
target_link_libraries(standalone_energy_reader)

install(TARGETS apmidg_sweep_pwrlim
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
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
install(TARGETS standalone_energy_reader
        RUNTIME DESTINATION bin
	DESTINATION ${CMAKE_INSTALL_BINDIR})
