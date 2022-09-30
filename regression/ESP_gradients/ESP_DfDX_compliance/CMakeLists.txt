# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "plato_analyze_input_deck.xml;plato_analyze_operations.xml;cuboid.csm;grad_check.py;finite_difference_utils.py")
Plato_add_test_files("${testFiles}")

# Set input file names for custom command
set( PYTHON_FILE "grad_check.py")
set( RELATIVE_TOLERANCE 1e-6 )
set( FINITE_DIFFERENCE_STEP 1e-6 )

# Create dummy input 
set( NUM_PROCS 1 )
set( IO_COMM_INDEX 0 )

SET(TEST_COMMAND "python ${PYTHON_FILE} ${RELATIVE_TOLERANCE} ${FINITE_DIFFERENCE_STEP} &> output.msg")
SET(CUSTOM_COMMAND "echo test has run")

# Add the test
Plato_add_custom_command_test(${TEST_COMMAND} ${TEST_NAME} ${NUM_PROCS} ${IO_COMM_INDEX} ${CUSTOM_COMMAND})
