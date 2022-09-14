# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "amgx.json;defines.xml;interface.xml;mpirun.source;plato_analyze_2_operations.xml;plato_dakota_input_deck.i;plato_main_input_deck.xml;plato_main_operations.xml;rocker.exo;evaluations_0/morph_input_0.i;evaluations_0/plato_analyze_2_input_deck_0.xml;evaluations_0/plato_main_input_deck_0.xml;evaluations_0/rocker_0.csm;evaluations_0/rocker_0.exo;evaluations_1/morph_input_1.i;evaluations_1/plato_analyze_2_input_deck_1.xml;evaluations_1/plato_main_input_deck_1.xml;evaluations_1/rocker_1.csm;evaluations_1/rocker_1.exo;evaluations_2/morph_input_2.i;evaluations_2/plato_analyze_2_input_deck_2.xml;evaluations_2/plato_main_input_deck_2.xml;evaluations_2/rocker_2.csm;evaluations_2/rocker_2.exo")
Plato_add_test_files("${testFiles}")

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Set file names for diff
set( OUT_FILE "dakota_multi_dimensional_parameter_study.dat")
set( GOLD_FILE "solution.txt.gold")

# set relative tolerance for diff test
set( NUMDIFF_USE_ABSOLUTE FALSE)
set( NUMDIFF_RELATIVE_TOLERANCE 1e-4 )

set( RUN_COMMAND "source ${CMAKE_CURRENT_BINARY_DIR}/mpirun.source" )

# Add the test
Plato_add_numdiff_test(${RUN_COMMAND} ${TEST_NAME} ${NUMDIFF_COMMAND} ${NUMDIFF_USE_ABSOLUTE} ${NUMDIFF_RELATIVE_TOLERANCE})

endif()
