
environment
  tabular_data
    tabular_data_file = 'rocker_multidim.dat'

method
  multidim_parameter_study
    partitions = 2 2

model
  single

variables
  continuous_design = 2
    lower_bounds 2.0   1.6
    upper_bounds 3.0   2.4
    descriptors  'Py'  'Px'

interface
    analysis_drivers = 'plato_dakota_plugin'
    direct
    asynchronous evaluation_concurrency 3

responses
  response_functions = 1
  no_gradients
  no_hessians
