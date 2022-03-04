begin service 1
   code platomain
   number_processors 1
end service

begin service 2
   code plato_analyze
   number_processors 1
end service
   
begin criterion 2
   type displacement
   displacement_direction 0 -1 0
   measure_magnitude false
   location_types sideset
   location_names ss_2
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
   minimum_ersatz_material_value 1e-3
end scenario   

begin objective
   type weighted_sum
   criteria 2 
   services 2 
   scenarios 1 
   weights 1 
end objective

begin constraint 1
   criterion 3
   relative_target .25
   service 1
   scenario 1
end constraint

begin output
   service 2
   data dispx dispy dispz vonmises
end output

begin boundary_condition 1
   type fixed_value
   location_type nodeset
   location_name ns_1
   degree_of_freedom dispx dispy dispz
   value 0 0 0
end boundary_condition

begin load 1
   type traction
   location_type sideset
   location_name ss_2
   value 0 -3e3 0
end load
         
begin material 1
   material_model isotropic_linear_elastic
   poissons_ratio .3
   youngs_modulus 1e8
end material

begin block 1
   material 1
end block

begin optimization_parameters
   max_iterations 20
   verbose false
   write_restart_file true
   number_buffer_layers 0
   filter_type kernel
   filter_radius_scale 4.48
   optimization_algorithm oc
   normalize_in_aggregator true
   output_frequency 100
   initial_density_value 0.25
end optimization_parameters


begin mesh
   name bolted_bracket.exo
end mesh
