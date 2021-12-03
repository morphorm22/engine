# Copy Input files from source to binary dir
set( testFiles "plato_analyze_0_input_deck_1.xml;rocker0.csm;rocker_0.exo")
Plato_add_test_files("${testFiles}")
