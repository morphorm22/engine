begin service 1  
  code platomain
  number_processors 1
end service

begin service 2
  code sierra_tf
  number_processors 1
end service

begin service 3
  code plato_esp
  number_processors 1
end service

begin criterion 1
  type temperature_matching
  search_nodesets 4
  match_nodesets  1 2 3 5
  ref_data_file gold.e
  temperature_field_name T
end criterion

begin scenario 1
  physics transient_thermal
  existing_input_deck aria1.i
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
   max_iterations 100 
   output_frequency 1
   optimization_algorithm rol_bound_constrained
   rol_subproblem_model lin_more
   hessian_type zero
   normalize_in_aggregator false
   csm_file aria1.csm
   num_shape_design_variables 1
   optimization_type shape
   verbose true
   limited_memory_storage 8
end optimization_parameters

begin mesh
   name aria1.exo
end mesh
