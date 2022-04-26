begin service 1
  code platomain
  number_processors 1
  number_ranks 1
end service

begin service 2
  code sierra_sd
  number_processors 4
  number_ranks 1
end service

begin service 3
  code sierra_sd
  number_processors 4
  number_ranks 1
end service

begin criterion 1
  type volume_average_von_mises
  block 2
end criterion

begin criterion 2
  type volume
end criterion

begin scenario 1
  physics steady_state_mechanics
  dimensions 3
  loads 1 2
  boundary_conditions 1
  material aluminum
  convert_to_tet10 true
end scenario

begin scenario 2
  physics steady_state_mechanics
  dimensions 3
  loads 3 4
  boundary_conditions 1 
  material aluminum
  convert_to_tet10 true
end scenario

begin objective
  type multi_objective
  criteria 1 1 2
  services 2 3 1
  scenarios 1 2 1
  weights 1 1 1
end objective

begin boundary_condition 1
    type fixed_value
    location_type sideset
    #location_name center_axis
    location_id 4
    degree_of_freedom dispx dispz dispy
    value 0 0 0
end boundary_condition

begin load 1
    type traction
    location_type sideset
    #location_name left_axis
    location_id 3
    value -1.0e3 0 0
end load

begin load 2
    type traction
    location_type sideset
    #location_name right_axis
    location_id 2
    value 1.0e3 0 0
end load

begin load 3
    type traction
    location_type sideset
    #location_name left_axis
    location_id 3
    value 0 -2.0e3 0
end load

begin load 4
    type traction
    location_type sideset
    #location_name right_axis
    location_id 2
    value 0 -2.0e3 0
end load
      
begin block 1
   material 1
   element_type tet10
   sub_block -1.2 -1.5 -1.0 1.2 1.5 1.0
end block

begin block 2
   material 1
   element_type tet10
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
    csm_file rocker_origin.csm
    mdps_partitions 2
    mdps_response_functions 3
end optimization_parameters

begin mesh
   name rocker_origin.exo
end mesh

