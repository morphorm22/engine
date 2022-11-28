# specify codes
if(PLATOSTATICS)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "interface.xml;lightmp.xml;localApp.xml;platoApp.xml;platomain.xml;unsmesh.gen;unsmesh.jou;rol_inputs.xml;ROL_output.txt.gold")
Plato_add_test_files("${testFiles}")

# Set number of processors for PlatoMain comm
set( PLATOMAIN_NP 3 )

# Set number of processors for Performer comms
set( PLATOSTATICS_NP 4 )

# Create list of comm sizes 
set( NUM_PROCS "${PLATOMAIN_NP} ${PLATOSTATICS_NP} ${PLATOSTATICS_NP}" )

# Specify which comm will provide the output for comparison
set( IO_COMM_INDEX 0 )

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Create the test and define the PlatoMain performer
set(APP_FILE platoApp.xml)
set(INPUT_FILE platomain.xml)
Plato_create_test(PLATO_NEW_TEST ${PLATOMAIN_BINARY} ${PLATOMAIN_NP} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 1 )
set(APP_FILE localApp.xml)
set(INPUT_FILE lightmp.xml)
Plato_add_performer(PLATO_NEW_TEST ${PLATOSTATICS_BINARY} ${PLATOSTATICS_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 2 )
set(APP_FILE localApp.xml)
set(INPUT_FILE lightmp.xml)
Plato_add_performer(PLATO_NEW_TEST ${PLATOSTATICS_BINARY} ${PLATOSTATICS_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Add the test
set(INPUT_MESH unsmesh.gen)
set( OUT_FILE "ROL_output.txt")
set( GOLD_FILE "ROL_output.txt.gold")
set( NUMDIFF_USE_ABSOLUTE FALSE)
set( NUMDIFF_RELATIVE_TOLERANCE 1e-4 )
message(STATUS "Run command: ${PLATO_TEST_COMMAND}")
# Add the test
Plato_add_parallel_numdiff_test(
    ${PLATO_NEW_TEST} ${TEST_NAME} ${NUMDIFF_COMMAND} ${NUMDIFF_USE_ABSOLUTE} ${NUMDIFF_RELATIVE_TOLERANCE} ${NUM_PROCS} ${IO_COMM_INDEX} ${INPUT_MESH})
set_property(TEST ${TEST_NAME} PROPERTY LABELS "small")

endif()
