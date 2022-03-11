begin service 1
  code platomain
  number_processors 1
  number_ranks 1
end service

begin criterion 1
  type volume
end criterion

begin scenario 1
  physics steady_state_mechanics
  dimensions 3
  loads 1
  boundary_conditions 1
end scenario

begin objective
  type single_criterion
  criteria 1
  services 1 
  scenarios 1
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
    dakota_workflow mdps
    concurrent_evaluations 3
    num_shape_design_variables 2
    csm_file rocker.csm
    mdps_partitions 2
    mdps_response_functions 1
end optimization_parameters

begin mesh
   name rocker.exo
end mesh

