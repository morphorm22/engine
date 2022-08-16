# specify codes
if(PLATOPROXY)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "interface.xml;rol_inputs.xml;ROL_gradient_check_output.gold.txt;confirm_gradient_convergence_trend.py")
Plato_add_test_files("${testFiles}")

# Set number of processors for PlatoMain comm
set( PLATOMAIN_NP 1 )

# Set number of processors for Performer comms
set( PROXY_NP 1 )

# Create list of comm sizes 
set( NUM_PROCS "${PLATOMAIN_NP} ${PROXY_NP}" )

# Specify which comm will provide the output for comparison
set( IO_COMM_INDEX 0 )

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Set input file names for custom command
set( PYTHON_FILE "confirm_gradient_convergence_trend.py")
set( INFNORM_IGNORE_ROWS 1 )
set( INFNORM_RELATIVE_TOLERANCE 1e-2 )
set( OUT_FILE "ROL_gradient_check_output.txt")
set( GOLD_FILE "ROL_gradient_check_output.gold.txt")
set( LAST_ROW 6 )

# Create simple regression test and define the PlatoMain performer
Plato_create_simple_test(PLATO_NEW_TEST ${PLATOMAIN_BINARY} ${PLATOMAIN_NP} ${INTERFACE_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 1 )
Plato_add_simple_performer(PLATO_NEW_TEST ${PLATOROCEKTDESIGN_BINARY} ${PROXY_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE})

SET(CUSTOM_COMMAND "python ${PYTHON_FILE} ${INFNORM_IGNORE_ROWS} ${INFNORM_RELATIVE_TOLERANCE} ${OUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${GOLD_FILE} ${LAST_ROW}")

# Add the test
Plato_add_custom_command_test(${PLATO_NEW_TEST} ${TEST_NAME} ${NUM_PROCS} ${IO_COMM_INDEX} ${CUSTOM_COMMAND})
set_property(TEST ${TEST_NAME} PROPERTY LABELS "small")


endif()


