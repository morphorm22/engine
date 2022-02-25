begin service 1
  code platomain
  path /ascldap/users/rvierte/plato/develop-env/platoengine/spack-build-i24ei2h/apps/services/PlatoMain
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
  loads 1 2
  boundary_conditions 1
  material 1
  minimum_ersatz_material_value 1e-9
  linear_solver_tolerance 5e-8
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

begin load 1
    type traction
    location_type sideset
    location_name ss_1
    value 0 -1e6 0
end load

begin load 2
    type traction
    location_type sideset
    location_name ss_2
    value 0 -1e6 0
end load
      
begin constraint 1
  criterion 2
  relative_target 0.3
  type less_than
  service 1
end constraint

begin block 1
   material 1
end block

begin material 1
   material_model orthotropic_linear_elastic 
   poissons_ratio_xy .28
   poissons_ratio_xz .28
   poissons_ratio_yz .4
   shear_modulus_xy 6.6e9
   shear_modulus_xz 6.6e9
   shear_modulus_yz 3.928e9
   youngs_modulus_x 126e9
   youngs_modulus_y 11e9
   youngs_modulus_z 11e9
end material

begin optimization_parameters
   filter_radius_absolute .11763
   max_iterations 20 
   output_frequency 1000 
   optimization_algorithm oc
   discretization density 
   initial_density_value .5
   normalize_in_aggregator false
end optimization_parameters

begin mesh
   name bolted_bracket.exo
end mesh

