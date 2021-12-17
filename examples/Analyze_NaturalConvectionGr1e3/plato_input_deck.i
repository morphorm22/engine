begin service 1
  code platomain
  number_processors 1
end service

begin service 2
  code plato_analyze
  number_processors 1
end service

begin output
  service 2
  native_service_output false
end output

begin criterion 1
  type mean_temperature
end criterion

begin criterion 2
  type volume
end criterion

begin scenario 1
  physics steady_state_incompressible_fluids
  heat_transfer natural
  steady_state_tolerance 2.5e-2
  time_step_safety_factor 0.9
  dimensions 2
  boundary_conditions 1 2 3 4 5 6
  material 1
end scenario

begin objective
  scenarios 1
  criteria 1
  services 2
  type weighted_sum
  weights 1
end objective

begin constraint 1
  criterion 2
  relative_target 0.75 // 75% of the domain will be fluid and 25% will be solid
  type less_than
  service 1
  scenario 1
end constraint

begin boundary_condition 1
  type fixed_value
  location_type sideset
  location_name x_plus
  degree_of_freedom velx vely
  value 0.0 0.0
end boundary_condition

begin boundary_condition 2
  type fixed_value
  location_type sideset
  location_name x_minus
  degree_of_freedom velx vely
  value 0.0 0.0
end boundary_condition

begin boundary_condition 3
  type fixed_value
  location_type sideset
  location_name y_plus
  degree_of_freedom velx vely
  value 0.0 0.0
end boundary_condition

begin boundary_condition 4
  type fixed_value
  location_type sideset
  location_name y_minus
  degree_of_freedom velx vely
  value 0.0 0.0
end boundary_condition

begin boundary_condition 5
  type zero_value
  location_type sideset
  location_name press
  degree_of_freedom press
end boundary_condition

begin boundary_condition 6
  type fixed_value
  location_type sideset
  location_name temp
  degree_of_freedom temp
  value 1.0
end boundary_condition

begin block 1
  material 1
  name block_1
end block

begin material 1
  material_model natural_convection
  prandtl_number 0.7
  grashof_number 0 1e3
end material

begin optimization_parameters
  optimization_algorithm mma
  discretization density
  max_iterations 5
  mma_move_limit 0.25
  filter_radius_scale 2.75
end optimization_parameters

begin mesh
  name heat_sink.exo
end mesh
