# Copy Input files from source to binary dir
set( testFiles "plato_analyze_1_input_deck_1.xml;rocker1.csm;rocker_1.exo")
Plato_add_test_files("${testFiles}")
