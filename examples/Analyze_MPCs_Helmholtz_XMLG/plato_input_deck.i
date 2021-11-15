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
  minimum_ersatz_material_value 1e-9
end criterion

begin criterion 2
  type volume
end criterion

begin scenario 1
  physics steady_state_mechanics
  dimensions 3
  loads 1
  boundary_conditions 1
  assemblies 1 2 3
  material 1
  minimum_ersatz_material_value 1e-9
  tolerance 1e-8
end scenario

begin objective
  type weighted_sum
  criteria 1
  services 2
  scenarios 1
  weights 1
end objective

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

begin assembly 1
    type tied
    child_nodeset child_1
    parent_block 1
end assembly
begin assembly 2
    type tied
    child_nodeset child_2
    parent_block 1
end assembly

begin load 1
    type traction
    location_type sideset
    location_name ss_1
    value 0 -3e3 0
end load
      
begin constraint 1
  criterion 2
  relative_target 0.5
  type less_than
  service 1
  scenario 1
end constraint

begin block 1
   material 1
end block
begin block 2
   material 2
end block
begin block 3
   material 2
end block

begin material 1
   material_model isotropic_linear_elastic 
   poissons_ratio .33
   youngs_modulus 68.0e9
end material
begin material 2
   material_model isotropic_linear_elastic 
   poissons_ratio .27
   youngs_modulus 193.0e9
end material

begin optimization_parameters
   filter_type helmholtz
   filter_radius_absolute 0.13
   boundary_sticking_penalty -1.0
   max_iterations 3
   output_frequency 1000 
   optimization_algorithm oc
   discretization density 
   initial_density_value 0.5
   normalize_in_aggregator false
   fixed_block_ids 2 3
end optimization_parameters

begin mesh
   name bolted_bracket_arm.exo
end mesh

