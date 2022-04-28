# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "SierraTF_TempMatch.ref.exo;aria1.csm;aria1.i;defines.xml;gold.e;interface.xml;inverseInput.xml;plato_esp_input_deck.xml;plato_esp_operations.xml;plato_main_input_deck.xml;plato_main_operations.xml;sierra_tf_2_operations.xml")
Plato_add_test_files("${testFiles}")

set( PRETEST_COMMAND "plato-cli geometry esp --input aria1.csm --output-model aria1_opt.csm --output-mesh aria1.exo --tesselation aria1.eto")

# Set number of processors for PlatoMain comm
set( PLATOMAIN_NP 1 )

# Set number of processors for Performer comms                                                                                                     
set( PLATOESP_NP 1 )

# Set number of processors for Performer comms
set( TF_NP 1 )
set( TF_BINARY plato_tf_main )

# Create list of comm sizes 
set( NUM_PROCS "${PLATOMAIN_NP} ${PLATOESP_NP} ${TF_NP}" )

# Specify which comm will provide the output for comparison
set( IO_COMM_INDEX 0 )

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Create the test and define the PlatoMain performer
set(APP_FILE plato_main_operations.xml)
set(INPUT_FILE plato_main_input_deck.xml)
Plato_create_test(PLATO_NEW_TEST ${PLATOMAIN_BINARY} ${PLATOMAIN_NP} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 1 )
set(APP_FILE sierra_tf_2_operations.xml)
set(INPUT_FILE "-i aria1.i -opt inverseInput.xml")
Plato_add_performer(PLATO_NEW_TEST ${TF_BINARY} ${TF_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

# Define/add a performer
set( LOCAL_COMM_ID 2 )
set(APP_FILE plato_esp_operations.xml)
set(INPUT_FILE plato_esp_input_deck.xml)
Plato_add_performer(PLATO_NEW_TEST ${PLATOESP_BINARY} ${PLATOESP_NP} ${LOCAL_COMM_ID} ${INTERFACE_FILE} ${APP_FILE} ${INPUT_FILE})

set( PLATO_NEW_TEST ${PRETEST_COMMAND} ${PLATO_NEW_TEST} )

# Add the test
set(INPUT_MESH SKIP_DECOMP)
set(OUTPUT_MESH SKIP_EXODIFF)
Plato_add_test("${PLATO_NEW_TEST}" ${TEST_NAME} ${NUM_PROCS} ${IO_COMM_INDEX} ${INPUT_MESH} ${OUTPUT_MESH})
