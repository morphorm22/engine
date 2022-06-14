# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "plato_input_deck.i;simple_mock.csm")
Plato_add_test_files("${testFiles}")

# Set xml generator command
set(XMLGEN_COMMAND "../../base/src/input_generator/XMLGenerator plato_input_deck.i")

# Set file names for diff
set( OUT_FILE "simple_mock_opt.csm")
set( GOLD_FILE "gold.csm")

# set tolerance for diff test
set( NUMDIFF_ABSOLUTE_TOLERANCE 1e-10 )

SET( NUMDIFF_USE_ABSOLUTE TRUE )

# Add the test
Plato_add_xmlgen_numdiff_test(${TEST_NAME} ${XMLGEN_COMMAND} ${NUMDIFF_COMMAND} ${NUMDIFF_ABSOLUTE_TOLERANCE} ${NUMDIFF_USE_ABSOLUTE})
