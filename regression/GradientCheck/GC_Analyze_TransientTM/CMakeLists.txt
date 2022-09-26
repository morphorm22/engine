# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "interface.xml;analyzeInput.xml;analyzeApp.xml;platoApp.xml;platoMain.xml;amgx.json;bolted_bracket.exo;rol_inputs.xml;ROL_gradient_check_output.gold.txt;confirm_gradient_convergence_trend.py")
Plato_add_test_files("${testFiles}")

# Set number of processors for PlatoMain comm
set( PLATOMAIN_NP 1 )

# Set number of processors for Performer comms
set( ANALYZE_NP 1 )

# Create list of comm sizes 
set( NUM_PROCS "${PLATOMAIN_NP} ${ANALYZE_NP}" )

# Specify which comm will provide the output for comparison
set( IO_COMM_INDEX 0 )

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Create the test and define the PlatoMain performer
set(APP_FILE platoApp.xml)
set(INPUT_FILE platoMain.xml)
Plato_create_test(PLATO_NEW_TEST ${PLATOMAIN_BINARY} ${PLATOMAIN_NP} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 1 )
set(APP_FILE analyzeApp.xml)
set(INPUT_FILE --input-config=analyzeInput.xml)
Plato_add_performer(PLATO_NEW_TEST ${ANALYZE_BINARY} ${ANALYZE_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Set input file names for custom command
set( PYTHON_FILE "confirm_gradient_convergence_trend.py")
set( INFNORM_IGNORE_ROWS 1 )
set( INFNORM_RELATIVE_TOLERANCE 1e-1 )
set( OUT_FILE "ROL_gradient_check_output.txt")
set( GOLD_FILE "ROL_gradient_check_output.gold.txt")
set( LAST_ROW 6 )

#SET(CUSTOM_COMMAND "python ${PYTHON_FILE} ${INFNORM_IGNORE_ROWS} ${INFNORM_RELATIVE_TOLERANCE} ${OUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${GOLD_FILE} ${LAST_ROW}")

# Add the test
#Plato_add_custom_command_test(${PLATO_NEW_TEST} ${TEST_NAME} ${NUM_PROCS} ${IO_COMM_INDEX} ${CUSTOM_COMMAND})
Plato_add_expect_fail_test(${PLATO_NEW_TEST} ${TEST_NAME} ${NUM_PROCS} ${IO_COMM_INDEX} "Transient thermomechanics is not currently supported." "Alexa_1")
set_property(TEST ${TEST_NAME} PROPERTY LABELS "small")

endif()
