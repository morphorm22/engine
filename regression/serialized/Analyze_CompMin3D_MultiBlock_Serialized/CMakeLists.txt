# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

set( testFolder "regression/serialized/${TEST_NAME}/")

# Copy Input files from source to binary dir
set( testFiles "plato_input_deck.i;bb_2block.exo;xml_comparison_files.txt;serialize_comparison_files.txt;numdiff_comparison_files.txt;")
Plato_add_test_files("${testFiles}")

add_custom_target(${TEST_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${testFolder}/gold ${CMAKE_BINARY_DIR}/${testFolder}/gold
    DEPENDS ${MY_TARGET})

# Set xml generator command
set(XMLGEN_COMMAND "../../../base/src/input_generator/XMLGenerator plato_input_deck.i")

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Set input file names for custom command
set( PYTHON_FILE "../../utility/full_regression_test.py")
set( IN_FILE "xml_comparison_files.txt serialize_comparison_files.txt numdiff_comparison_files.txt")
set( NUMDIFF_TOLERANCE 1e-3 ) 

SET(CUSTOM_COMMAND "python ${PYTHON_FILE} ${NUMDIFF_TOLERANCE} ${IN_FILE}")


# Add the test
Plato_add_xmlgen_custom_command_test(${TEST_NAME} ${XMLGEN_COMMAND} ${CUSTOM_COMMAND})
set_property(TEST ${TEST_NAME} PROPERTY ENVIRONMENT "PLATO_SAVE_TO_XML=1")

set_property(TEST ${TEST_NAME} PROPERTY LABELS "medium")

endif()
