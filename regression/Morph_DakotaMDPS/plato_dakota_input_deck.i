# input file for dakota multi-dimensional parameter study (MDPS) workflow

 environment
   tabular_data
       tabular_data_file = 'dakota_multi_dimensional_parameter_study.dat'

 method
   multidim_parameter_study
       partitions = 2 2

 variables
   continuous_design = 2
       descriptors 'py' 'px'
       lower_bounds 2.0 1.6
       upper_bounds 3.0 2.4
       initial_point 2.0 2.0

 interface
   analysis_drivers = 'plato_dakota_plugin'
   direct
   asynchronous evaluation_concurrency 3

 responses
   response_functions = 1
   no_gradients
   no_hessians
