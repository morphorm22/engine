# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "interface.xml;analyzeInput.xml;analyzeApp.xml;platoApp.xml;platoMain.xml;amgx.json;tm2.exo;rol_inputs.xml;ROL_output.txt.gold")
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
Plato_create_test(PLATO_TEST_COMMAND ${PLATOMAIN_BINARY} ${PLATOMAIN_NP} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 1 )
set(APP_FILE analyzeApp.xml)
set(INPUT_FILE --input-config=analyzeInput.xml)
Plato_add_performer(PLATO_TEST_COMMAND ${ANALYZE_BINARY} ${ANALYZE_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Files and tolerances
set( OUT_FILE "ROL_output.txt")
set( GOLD_FILE "ROL_output.txt.gold")
set( NUMDIFF_USE_ABSOLUTE FALSE)
set( NUMDIFF_RELATIVE_TOLERANCE 1e-4 )

# Add the test
Plato_add_numdiff_test(${PLATO_TEST_COMMAND} ${TEST_NAME} ${NUMDIFF_COMMAND} ${NUMDIFF_USE_ABSOLUTE} ${NUMDIFF_RELATIVE_TOLERANCE})
set_property(TEST ${TEST_NAME} PROPERTY LABELS "large")

# Note: The ROL_output.txt.gold was generated with AMGX and may need to be updated when this test is enabled

endif()
