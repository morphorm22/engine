begin service 1
   code platomain
   number_processors 1
end service

begin service 2
   code plato_analyze
   number_processors 1
end service
   
begin service 3
   code plato_analyze
   number_processors 1
end service

begin criterion 2
   type displacement
   displacement_direction 0 -1 0
   measure_magnitude true
   location_type sideset
   location_name ss1
end criterion
      
begin criterion 3
   type volume
end criterion

begin scenario 1
   physics steady_state_mechanics
   dimensions 3
   loads 1
   boundary_conditions 1
   material 1
end scenario   

begin objective
   type weighted_sum
   criteria 3 
   services 3 
   scenarios 1 
   weights 1 
end objective

begin constraint 1
   criterion 2
   absolute_target .08
   service 2
   scenario 1
end constraint

begin output
   service 2
   data dispx dispy dispz vonmises
end output

begin boundary_condition 1
   type fixed_value
   location_type nodeset
   location_name ss2
   degree_of_freedom dispx dispy dispz
   value 0 0 0
end boundary_condition

begin load 1
   type traction
   location_type sideset
   location_name ss1
   value 0 -1e6 0
end load
         
begin material 1
   material_model isotropic_linear_elastic
   poissons_ratio .33
   youngs_modulus 1e9
end material

begin block 1
   material 1
end block

begin optimization_parameters
   max_iterations 10
   verbose false
   write_restart_file true
   number_buffer_layers 0
   filter_type kernel
   filter_radius_absolute .4
   optimization_algorithm rol_augmented_lagrangian
   normalize_in_aggregator false
   output_frequency 100
   //initial_density_value 1.0
end optimization_parameters


begin mesh
   name mesh.exo
end mesh
