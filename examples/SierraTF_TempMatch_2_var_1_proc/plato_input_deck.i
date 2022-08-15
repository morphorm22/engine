// Optimizer service
begin service 1  
  code platomain
  number_processors 1
end service

// Service for calculating temperature match
begin service 2
  code sierra_tf
  number_processors 1
  path /fgs/bwclark/sierra/bin/linux-gcc-7.2.0-openmpi-4.0.5/release/plato_tf_main
end service

// Service for calculating CAD parameter sensitivities
begin service 3
  code plato_esp
  // There are 3 design variables in the .csm file--this number should match that.
  number_processors 2
end service

begin criterion 1
   type temperature_matching
   search_nodesets 3
   match_nodesets  1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22
   ref_data_file gold.exo
   temperature_field_name T
end criterion

begin scenario 1
   physics transient_thermal
   existing_input_deck aria.i
end scenario

begin objective
  type weighted_sum
  criteria 1
  services 2
  shape_services 3
  scenarios 1
  weights 1
end objective

begin output
   service 2
   data temperature
end output

begin optimization_parameters
   max_iterations 15
   output_frequency 5
   //optimization_algorithm ksbc
   optimization_algorithm rol_bound_constrained
   rol_subproblem_model lin_more
   hessian_type zero
   normalize_in_aggregator false
   csm_file temperature_matching.csm
   num_shape_design_variables 2
   optimization_type shape
   verbose true
   number_buffer_layers 0
end optimization_parameters

begin mesh
   name temperature_matching.exo
end mesh
