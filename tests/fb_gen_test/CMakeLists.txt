add_custom_target(
    test_edl_circular_buffer_generator
    ALL
    DEPENDS edl_circular_buffer_generator
    COMMAND ${CMAKE_BINARY_DIR}/output/debug/edl_circular_buffer_generator.exe -sourceFile C:/Dev/Secretarium/core2/secretarium/interfaces/tee/logger_test_enclave.edl -preparseFile build/preproc/preproc.edl -flatbufferPath build/preproc -headerPath build/preproc -cppPath build/preproc -flatbuffer_class_name logger -generated_namespace logger -includePath C:/Dev/Secretarium/core2/secretarium/interfaces/tee^C:/Dev/Secretarium/core2/secretarium/libraries/logger/common/include^C:/Dev/Secretarium/core2/secretarium/libraries/miscellaneous/common/include
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})