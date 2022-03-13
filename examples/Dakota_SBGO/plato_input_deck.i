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

begin criterion 1
  type mechanical_compliance
  minimum_ersatz_material_value 1e-3
end criterion

begin criterion 2
  type volume
end criterion

begin scenario 1
  physics steady_state_mechanics
  dimensions 3
  loads 1 2
  boundary_conditions 1
  material 1
  minimum_ersatz_material_value 1e-3
  tolerance 5e-8
end scenario

begin objective
  type multi_objective
  criteria 1 2
  services 2 2
  scenarios 1 1
  weights 1 1
end objective

begin boundary_condition 1
    type fixed_value
    location_type sideset
    location_name center_axis
    degree_of_freedom dispx dispz dispy
    value 0 0 0
end boundary_condition

begin load 1
    type traction
    location_type sideset
    location_name left_axis
    value 0 2.0e3 0
end load

begin load 2
    type traction
    location_type sideset
    location_name right_axis
    value 0 1.0e3 0
end load
      
begin block 1
   material 1
end block

begin material 1
   material_model isotropic_linear_elastic 
   poissons_ratio .33
   youngs_modulus 1.0e9
end material

begin optimization_parameters
    optimization_type dakota
    dakota_workflow sbgo
    concurrent_evaluations 3
    num_shape_design_variables 2
    csm_file rocker.csm
    sbgo_max_iterations 1
    moga_population_size 300
    moga_max_function_evaluations 20000
    moga_niching_distance 0.02
    num_sampling_method_samples 12
end optimization_parameters

begin mesh
   name rocker.exo
end mesh

