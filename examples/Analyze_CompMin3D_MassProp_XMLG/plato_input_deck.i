begin service 1
  code platomain
  number_processors 1
  number_ranks 1
end service

begin service 2
  code plato_analyze
  number_processors 1
  number_ranks 1
end service

begin service 3
  code plato_analyze
  number_processors 1
  number_ranks 1
end service

begin criterion 1
  type mechanical_compliance
  minimum_ersatz_material_value 1e-3
end criterion

begin criterion 2
  type volume
end criterion

begin criterion 3
  type mass_properties
  cgx gold 1.8 weight 1
  cgy gold 0.5 weight 1
end criterion

begin scenario 1
  physics steady_state_mechanics
  dimensions 3
  loads 1
  boundary_conditions 1
  material 1
  minimum_ersatz_material_value 1e-3
  tolerance 5e-8
end scenario

begin objective
  type weighted_sum
  criteria 1 3
  services 2 3
  scenarios 1 1
  weights 1 10000
end objective

// Use this objective if enforcing the CG
// constraints as a constraint and not as
// part of the objective.
//begin objective
//  type weighted_sum
//  criteria 1 
//  services 2 
//  scenarios 1 
//  weights 1
//end objective

begin output
    service 2
   output_data true
   data dispx dispy dispz
end output

begin boundary_condition 1
    type fixed_value
    location_type nodeset
    location_name ns_1
    degree_of_freedom dispx dispz dispy
    value 0 0 0
end boundary_condition

begin load 1
    type traction
    location_type sideset
    location_name ss_2
    value 0 -3e3 0
end load
      
begin constraint 1
  criterion 2
  relative_target 0.35
  type less_than
  service 1
  scenario 1
end constraint

// If you would rather run the CG constraints as 
// a constraint and not part of the objective 
// use this constraint definition.
//begin constraint 2
//  criterion 3
//  relative_target 0
//  service 3
//  scenario 1
//end constraint

begin block 1
   material 1
end block

begin block 2
   material 1
end block

begin material 1
   material_model isotropic_linear_elastic 
   poissons_ratio .3
   youngs_modulus 1e8
end material

begin optimization_parameters
   enforce_bounds true
   mma_use_ipopt_sub_problem_solver true
   //filter_radius_absolute .103
   //filter_radius_absolute .358
   //filter_radius_scale 4.48
   filter_radius_scale 4.48
   //filter_type helmholtz
   filter_type kernel
   max_iterations 10 
   output_frequency 1000 
   optimization_algorithm mma
   verbose true
   discretization density 
   initial_density_value .4
   normalize_in_aggregator false
   fixed_block_ids 2
end optimization_parameters

begin mesh
   name bolted_bracket_fixed_08.exo
end mesh

