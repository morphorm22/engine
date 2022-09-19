# Name the test with the directory name
Plato_new_test(TEST_NAME)

# Copy Input files from source to binary dir
set( testFiles "aria1.csm;aria1.i;gold.e;plato_input_deck.i")
Plato_add_test_files("${testFiles}")

# write out the path info in the plato input deck
Plato_add_text_to_file_after_keyword("plato_input_deck.i" "platomain" "path ${PLATOMAIN_BINARY}\n")

# Set xml generator command
set(XMLGEN_COMMAND "../../base/src/input_generator/XMLGenerator plato_input_deck.i")

# Set file names for diff
set( OUT_FILE "aria1_opt.csm")
set( GOLD_FILE "solution.csm.gold")

# set relative tolerance for diff test
set( NUMDIFF_USE_ABSOLUTE FALSE)
set( NUMDIFF_RELATIVE_TOLERANCE 1e-2 )

# Add the test
Plato_add_xmlgen_numdiff_test(${TEST_NAME} ${XMLGEN_COMMAND} ${NUMDIFF_COMMAND} ${NUMDIFF_USE_ABSOLUTE} ${NUMDIFF_RELATIVE_TOLERANCE})

