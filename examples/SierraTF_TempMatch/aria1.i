Begin SIERRA Bar

    Title Aria Transient Bar

$---------------------------------------------------
$   Define material properties to be used
$---------------------------------------------------
    # Aluminum
    Begin Aria Material MatA
        Density              = Constant rho = 2702. # kg/m^3, at 20 C
        Specific Heat        = Constant cp  = 903.  # J/kg-K, at 20 C
        Thermal Conductivity = Constant k = 10000000.  # W/mK
       $ Thermal Conductivity = Constant k = 200.  # W/mK
        Heat Conduction      = Basic
    End   Aria Material MatA

$------------------------------------------------------------
$   Define linear solver settings
$   - Conjugate Gradient method with Jacobi Preconditioning
$------------------------------------------------------------
$------------------------------------------------------------
    BEGIN TPETRA EQUATION SOLVER  SOLVE_TEMPERATURE_CG
      BEGIN CG SOLVER
        BEGIN JACOBI PRECONDITIONER
        END
        MAXIMUM ITERATIONS = 500
        RESIDUAL SCALING = NONE
        CONVERGENCE TOLERANCE = 1.000000e-15
      END
    END TPETRA EQUATION SOLVER

$---------------------------------------------------
$   Specify mesh name and settings
$---------------------------------------------------
    Begin Finite Element Model FEModel
        Database Name = aria1.exo
	decomposition method =  rcb

$---------------------------------------------------
$       Assign material properties to element blocks
$---------------------------------------------------
        Use Material MatA for block_1
     $   Use Material MatB for block_2
     $   Use Material MatC for block_3

    End   Finite Element Model FEModel

$--------------------------------------------------------
$   Procedure domain - solution control, region settings
$--------------------------------------------------------

    Begin Procedure AriaProcedure

$---------------------------------------------------
$       Define temporal solution parameters
$---------------------------------------------------
        Begin Solution Control Description
            Use System Main
            Begin System Main
                Simulation Start Time            = 0.0
                Simulation Termination Time      = 10.0
                Begin Transient solution_block_1
                    Advance AriaRegion
                End   Transient solution_block_1
            End   System Main
   
$---------------------------------------------------
$           Specify time integration settings
$           Note: There could be several solution 
$                 blocks changing these parameters
$---------------------------------------------------
            Begin Parameters for Transient Solution_Block_1
                Start Time       = 0.0                      # seconds
                Termination Time =  10.0                     # seconds
                Begin Parameters for Aria Region AriaRegion
                    Time Step Variation = Fixed
                    Initial Time Step Size = 0.5           # seconds
                    Time Integration Method = First_Order
                End   Parameters for Aria Region AriaRegion
            End   Parameters for Transient Solution_Block_1
        End   Solution Control Description

$------------------------------------------------------
$       Region domain - EQs, BCs, ICs, post-processing
$------------------------------------------------------

        Begin Aria Region AriaRegion

$---------------------------------------------------
$           Specify which linear solver to use
$---------------------------------------------------
            Use Linear Solver solve_temperature_cg

$---------------------------------------------------
$           Define nonlinear solver parameters
$---------------------------------------------------
            Nonlinear Solution Strategy  = Newton
            Maximum Nonlinear Iterations = 10
            Nonlinear Residual Tolerance = 1.e-6
            Nonlinear Relaxation Factor  = 1.0 # default

            Use DOF Averaged Nonlinear Residual
            Accept Solution After Maximum Nonlinear Iterations = False

$-----------------------------------------------------------
$           Specify which mesh model to use for this region
$-----------------------------------------------------------
            Use Finite Element Model FEModel

$---------------------------------------------------
$           Specify equations to solve
$---------------------------------------------------
            EQ Energy for Temperature on all_blocks using Q1 with Src Mass Diff 
            Source for energy on all_blocks = Optimization value = 0

$---------------------------------------------------
$           Specify initial conditions
$---------------------------------------------------
            IC for Temperature on all_blocks = Optimization value = 0

$---------------------------------------------------
$           Specify boundary conditions
$---------------------------------------------------
            BC flux for energy on surface_2 = Constant  value = 5e6
            #BC flux for energy on surface_2 = Constant  value = 1e6
            BC dirichlet for temperature on surface_3 = constant value = 20.

            Compute Shape Gradient on surface_2

$---------------------------------------------------
$---------------------------------------------------
$           Specify postprocessors
$---------------------------------------------------
            Postprocess Thermal_Conductivity on all_blocks using P0

$---------------------------------------------------
$           Define contents of binary plot file
$---------------------------------------------------
            Begin Results Output AriaOutput
                Database Name = aria1_output.e
                At Step 0 Interval = 1
                Title Aria: Bar
                Nodal Variables = Solution->Temperature as T
                Element Variables = pp->Thermal_Conductivity as k_e
                Global Variables = time_step as dt
     		Property Compose_Results = yes
            End   Results Output AriaOutput

        End   Aria Region AriaRegion

    End   Procedure AriaProcedure

End   SIERRA Bar
