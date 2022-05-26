begin service 1
  code platomain
  number_processors 1
  number_ranks 1
end service

begin service 2
  code gemma
  path /projects/gemma_user/develop/RedHat-Generic-NoGPU/bin/
  type system_call
  number_processors 1
end service

begin criterion 1
  type system_call
  data_group 1
  data_extraction_operation max
  data_file gemma_matched_power_balance_input_deck_power_balance.dat 
end criterion

begin scenario 1
  physics electromagnetics
  dimensions 3
  material 1
  cavity_radius 0.1016
  cavity_height 0.1016
  frequency_min 1e9
  frequency_max 2e9
  frequency_step 1e7
end scenario

begin objective
  type single_criterion
  criteria 1
  services 2
  scenarios 1
end objective

begin block 1
   material 1
end block

begin material 1
   material_model electromagnetics
   conductivity 2.6e7
end material

begin optimization_parameters
    optimization_type dakota
    dakota_workflow mdps
    num_shape_design_variables 3
    concurrent_evaluations 3
    descriptors   slot_length  slot_width  slot_depth
    lower_bounds     0.01        0.001        0.001
    upper_bounds     0.10        0.010        0.010
    mdps_partitions    3            3           3
    mdps_response_functions 1
end optimization_parameters
