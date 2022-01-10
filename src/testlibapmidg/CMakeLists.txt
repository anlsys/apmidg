
project( ${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION "libapmidg" LANGUAGES C)

set(TESTSRC "testlibapmidg.c")


message(STATUS "Use ${libapmidg}")

add_executable(testapmidg ${TESTSRC})

set_target_properties(testapmidg PROPERTIES
        OUTPUT_NAME "testapmidg"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}" )

include_directories( "../libapmidg/" )

target_link_libraries(testapmidg libapmidg)


