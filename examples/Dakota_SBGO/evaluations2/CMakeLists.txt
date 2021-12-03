# Copy Input files from source to binary dir
set( testFiles "plato_analyze_2_input_deck_1.xml;rocker2.csm;rocker_2.exo")
Plato_add_test_files("${testFiles}")
