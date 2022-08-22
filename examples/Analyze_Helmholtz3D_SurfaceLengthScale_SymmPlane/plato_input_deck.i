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
  boundary_conditions 1 2
  material 1
  minimum_ersatz_material_value 1e-9
  linear_solver_tolerance 1e-8
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
  data dispx dispy dispz vonmises
end output

begin boundary_condition 1
    type fixed_value
    location_type sideset
    location_name fix
    degree_of_freedom dispx dispz dispy
    value 0 0 0
end boundary_condition

begin boundary_condition 2
    type fixed_value
    location_type sideset
    location_name sym
    degree_of_freedom dispx
    value 0
end boundary_condition

begin load 1
    type traction
    location_type sideset
    location_name load
    value 0 -1e4 0
end load
      
begin constraint 1
  criterion 2
  relative_target 0.3
  type less_than
  service 1
  scenario 1
end constraint

begin block 1
   material 1
   name body
end block

begin material 1
   material_model isotropic_linear_elastic 
   poissons_ratio .3
   youngs_modulus 1e8
end material

begin optimization_parameters
   filter_type helmholtz
   filter_radius_absolute 0.0693
   boundary_sticking_penalty 1.0
   symmetry_plane_location_names sym
   max_iterations 20 
   output_frequency 1000 
   optimization_algorithm oc
   discretization density 
   initial_density_value .5
   normalize_in_aggregator false
   amgx_max_iterations 20000
end optimization_parameters

begin mesh
   name beam.exo
end mesh

