# specify codes
if(ANALYZE)

# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "plato_input_deck.i;rocker_origin.csm")
Plato_add_test_files("${testFiles}")

# Set xml generator command
set(XMLGEN_COMMAND "../../base/src/input_generator/XMLGenerator plato_input_deck.i")

# Set the name of the interface file
set( INTERFACE_FILE interface.xml )

# Set file names for diff
set( OUT_FILE "dakota_multi_dimensional_parameter_study.dat")
set( GOLD_FILE "solution.txt.gold")

# set relative tolerance for diff test
set( NUMDIFF_RELATIVE_TOLERANCE 1e-4 )

# Add the test
Plato_add_xmlgen_numdiff_test(${TEST_NAME} ${XMLGEN_COMMAND} ${NUMDIFF_COMMAND} ${NUMDIFF_RELATIVE_TOLERANCE})

endif()
