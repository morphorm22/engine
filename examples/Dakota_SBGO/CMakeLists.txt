# specify codes
if(ANALYZE)

add_subdirectory(evaluations0)
add_subdirectory(evaluations1)
add_subdirectory(evaluations2)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "amgx.json;defines.xml;interface.xml;plato_analyze_operations_1.xml;plato_dakota_input_deck.i;platomain_operations.xml;PlatoMain.xml;rocker.exo")
Plato_add_test_files("${testFiles}")

# set( PRETEST_COMMAND "plato-cli geometry esp --input rocker.csm --output-model rocker_opt.csm --output-mesh rocker.exo --tesselation rocker.eto")

# Set number of processors for PlatoMain
set( PLATOMAIN_NP 1 )

# Set number of processors for analyze_MPMD
set( ANALYZE_NP 1 )

# Set number of processors for PlatoEngineServices
set( SERVICES_NP 3 )

# Create list of comm sizes 
set( NUM_PROCS "${PLATOMAIN_NP} ${ANALYZE_NP} ${ANALYZE_NP} ${ANALYZE_NP} ${SERVICES_NP}" )

# Specify which comm will provide the output for comparison
set( IO_COMM_INDEX 0 )

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Set file names for diff
set( OUT_FILE "surrogate_based_global_optim_dakota.dat")
set( GOLD_FILE "surrogate_based_global_optim_dakota.dat.gold")

# Create the test and define the PlatoMain performer
set(APP_FILE platomain_operations.xml)
set(INPUT_FILE PlatoMain.xml)
Plato_create_test(PLATO_NEW_TEST ${PLATOMAIN_BINARY} ${PLATOMAIN_NP} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add performers
set( LOCAL_COMM_ID 1 )
set(APP_FILE plato_analyze_operations_1.xml)
set(INPUT_FILE --input-config=evaluations0/plato_analyze_0_input_deck_1.xml)
Plato_add_performer(PLATO_NEW_TEST ${ANALYZE_BINARY} ${ANALYZE_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

set( LOCAL_COMM_ID 2 )
set(INPUT_FILE --input-config=evaluations1/plato_analyze_1_input_deck_1.xml)
Plato_add_performer(PLATO_NEW_TEST ${ANALYZE_BINARY} ${ANALYZE_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

set( LOCAL_COMM_ID 3 )
set(INPUT_FILE --input-config=evaluations2/plato_analyze_2_input_deck_1.xml)
Plato_add_performer(PLATO_NEW_TEST ${ANALYZE_BINARY} ${ANALYZE_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

set( LOCAL_COMM_ID 4 )
set(APP_FILE platomain_operations.xml)
set(INPUT_FILE PlatoMain.xml)
Plato_add_performer(PLATO_NEW_TEST ${SERVICES_BINARY} ${SERVICES_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# set relative tolerance for diff test
set( NUMDIFF_RELATIVE_TOLERANCE 1e-7 )

# Add the test
Plato_add_numdiff_test("${PLATO_NEW_TEST}" ${TEST_NAME} ${NUMDIFF_COMMAND} ${NUMDIFF_RELATIVE_TOLERANCE} ${NUM_PROCS} ${IO_COMM_INDEX})

endif()
