# <Problem_Description>
#
# time marching
# Dirichlet boundary condition
# Tabular Time-dependent Flux Boundary condition
# Tabular conductivity 
# constant initial condition
# single material
#
# </Problem_Description>

Begin SIERRA Aria VOTD: Class-Model

    Title Aria Transient Training Model

$---------------------------------------------------
$   Define material properties to be used
$---------------------------------------------------
    Begin Aria Material aluminum
        Density              = Constant rho = 2.770E+03 # kg/m^3, at 20 C
        Specific Heat        = Constant cp  = 800.0     # J/kg-K, at 20 C
        Thermal Conductivity = Constant k   = 175.0     # W/mK
        Heat Conduction      = Basic
    End   Aria Material aluminum

$---------------------------------------------------
$   Linear function for unsteady heat flux
$---------------------------------------------------
    Begin Definition for Function unsteady_flux
        Type is Piecewise Linear
        Begin Values
            #t (s)  Q(W/m^2)
            0.0     0.0
            500.0   4.0e4
            6000.0  8.0e4
           12000.0 10.0e4
        End Values
    End   Definition for Function unsteady_flux

$------------------------------------------------------------
$   Define linear solver settings
$   - Conjugate Gradient method with Jacobi Preconditioning
$------------------------------------------------------------
    BEGIN TPETRA EQUATION SOLVER solve_temperature_cg
      BEGIN CG SOLVER
        BEGIN JACOBI PRECONDITIONER
        END
        MAXIMUM ITERATIONS = 500
        RESIDUAL SCALING = R0
        CONVERGENCE TOLERANCE = 1.000000e-10
      END
    END TPETRA EQUATION SOLVER

$---------------------------------------------------
$   Specify mesh name and settings
$---------------------------------------------------
    Begin Finite Element Model FEModel
        #Database Name = gold.e
        Database Name = temperature_matching.exo

$---------------------------------------------------
$       Assign material properties to element blocks
$---------------------------------------------------
        Use Material aluminum for block_1 

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
                Simulation Termination Time      = 12000.
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
                Start Time       = 0.0    # seconds
                Termination Time = 12000.0 # seconds
                Begin Parameters for Aria Region AriaRegion
                    Time Step Variation = Fixed
                    Initial Time Step Size = 1000  # seconds
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

$-----------------------------------------------------------
$           Specify which mesh model to use for this region
$-----------------------------------------------------------
            Use Finite Element Model FEModel

$---------------------------------------------------
$           Specify equations to solve
$---------------------------------------------------
            EQ Energy for Temperature on all_blocks using Q1 with Diff Mass
            Source for energy on all_blocks = Optimization value = 0

$---------------------------------------------------
$           Specify initial conditions
$---------------------------------------------------
         IC for Temperature on all_blocks = Constant value = 300


         Compute Shape Gradient on insulated_sideset

$---------------------------------------------------
$           Specify boundary conditions
$---------------------------------------------------
            Begin Heat Flux Boundary Condition fluxBC
                Add Surface flux_bc_sideset
                Flux Time Function = unsteady_flux
            End   Heat Flux Boundary Condition fluxBC

            Begin Temperature Boundary Condition tempBC
                Add Surface fixed_bc_sideset
                Temperature = 400. # K
            End   Temperature Boundary Condition tempBC

$---------------------------------------------------
$           Define contents of binary plot file
$---------------------------------------------------
            Begin Results Output AriaOutput
                Database Name = output.exo
                At Step 0 Interval = 1
                Title Aria: Transient Training Model
                Nodal Variables = Solution->Temperature as T
                Global Variables = time_step as dt
            End   Results Output AriaOutput

        End   Aria Region AriaRegion

    End   Procedure AriaProcedure

End   SIERRA Aria VOTD: Class-Model

