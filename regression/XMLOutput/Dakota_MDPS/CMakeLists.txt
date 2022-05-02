# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "plato_input_deck.i;xmlout.gold;rocker.csm")
Plato_add_test_files("${testFiles}")

# Set xml generator command
set(XMLGEN_COMMAND "../../../base/src/input_generator/XMLGenerator plato_input_deck.i")

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

set( CUSTOM_COMMAND "diff xmlout.gold xmlout.tmp" )
# Add the test
Plato_add_xmlgen_no_run_custom_command_test(${TEST_NAME} ${XMLGEN_COMMAND} ${CUSTOM_COMMAND} )


endif()



