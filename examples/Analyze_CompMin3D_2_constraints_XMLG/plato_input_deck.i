begin service 1
  code platomain
  number_processors 1
  update_problem true
end service

begin service 2
  code plato_analyze
  number_processors 1
  update_problem false
end service

begin service 3
  code plato_analyze
  number_processors 1
  update_problem false
end service

begin criterion 1
  type mechanical_compliance
  material_penalty_exponent 3.0
  minimum_ersatz_material_value 1e-6
end criterion

begin criterion 2
  type volume
end criterion

begin criterion 3
  type mechanical_compliance
  material_penalty_exponent 3.0
  minimum_ersatz_material_value 1e-6
end criterion

begin scenario 1
  physics steady_state_mechanics
  dimensions 3
  loads 1
  boundary_conditions 1 2 3
  material 1
  material_penalty_exponent 3.0
  minimum_ersatz_material_value 1e-6
  tolerance 1e-10
end scenario

begin scenario 2
  physics steady_state_mechanics
  dimensions 3
  loads 2
  boundary_conditions 1 2 3
  material 1
  material_penalty_exponent 3.0
  minimum_ersatz_material_value 1e-6
  tolerance 1e-10
end scenario

begin objective
  type weighted_sum
  criteria 1
  services 2
  scenarios 1
  weights 1e-4
end objective

begin output
   service 2
   output_data true
   data dispx dispy dispz vonmises
   native_service_output false
end output

begin output
   service 3
   output_data true
   data vonmises
   native_service_output false
end output

begin boundary_condition 1
    type fixed_value
    location_type sideset
    location_name ss_nox
    degree_of_freedom dispx
    value 0
end boundary_condition

begin boundary_condition 2
    type fixed_value
    location_type sideset
    location_name ss_top
    degree_of_freedom dispy
    value 0
end boundary_condition

begin boundary_condition 3
    type fixed_value
    location_type sideset
    location_name ss_zsym
    degree_of_freedom dispz
    value 0
end boundary_condition

begin load 1
    type traction
    location_type sideset
    location_name ss_load
    value 0 -5e4 0
end load

begin load 2
    type traction
    location_type sideset
    location_name ss_load
    value -1e4 0 0
end load

begin constraint 1
  criterion 2
  relative_target 0.4
  type less_than
  service 1
  scenario 1
end constraint

begin constraint 2
  criterion 3
  absolute_target 50.0
  type less_than
  service 3
  scenario 2
end constraint

begin block 1
   material 1
end block

begin material 1
   material_model isotropic_linear_elastic 
   poissons_ratio 0.3
   youngs_modulus 1e8
end material

begin optimization_parameters
   filter_type kernel_then_tanh
   filter_use_additive_continuation true
   filter_projection_start_iteration 150
   filter_projection_update_interval 10
   filter_heaviside_max 10.0
   filter_heaviside_min 1.0
   filter_heaviside_update 0.5
   filter_radius_scale 4.0
   filter_in_engine true

   optimization_type topology
   max_iterations 2 
   output_frequency 1000 
   discretization density 
   initial_density_value 0.4
   normalize_in_aggregator false
   enforce_bounds false
   problem_update_frequency 1
   verbose false

   optimization_algorithm ksal
   mma_move_limit 0.2
   mma_output_subproblem_diagnostics true
   mma_use_ipopt_sub_problem_solver true
   mma_max_sub_problem_iterations 200

   write_restart_file false
   filter_power 1.0
   output_method epu
   mma_asymptote_expansion 1.2
   mma_asymptote_contraction 0.7
   mma_control_stagnation_tolerance -1e-6
   mma_objective_stagnation_tolerance -1e-8
   mma_sub_problem_initial_penalty 0.05
   mma_sub_problem_penalty_multiplier 1.025
   mma_sub_problem_feasibility_tolerance 1e-4
   ks_max_trust_region_iterations 10
end optimization_parameters

begin mesh
   name lbracket3D.exo
end mesh

