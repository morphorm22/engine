# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "plato_input_deck.i;rocker.csm;confirm_gradient_convergence_trend.py;ROL_gradient_check_output.gold.txt")
Plato_add_test_files("${testFiles}")

# Set xml generator command
set(XMLGEN_COMMAND "../../../base/src/input_generator/XMLGenerator plato_input_deck.i")

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Set input file names for custom command
set( PYTHON_FILE "confirm_gradient_convergence_trend.py")
set( INFNORM_IGNORE_ROWS 2 )
set( INFNORM_RELATIVE_TOLERANCE 1e-2 )
set( OUT_FILE "ROL_gradient_check_output.txt")
set( GOLD_FILE "ROL_gradient_check_output.gold.txt")
set( LAST_ROW 6 )


SET(CUSTOM_COMMAND "python ${PYTHON_FILE} ${INFNORM_IGNORE_ROWS} ${INFNORM_RELATIVE_TOLERANCE} ${OUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${GOLD_FILE} ${LAST_ROW}")

# Add the test
Plato_add_xmlgen_custom_command_test(${TEST_NAME} ${XMLGEN_COMMAND} ${CUSTOM_COMMAND})
Plato_disable_test(${TEST_NAME} "Reason unclear, possibly issues with ESP 1.21")

endif()
