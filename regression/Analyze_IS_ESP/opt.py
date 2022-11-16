import os
import sys
import exodus
import PlatoServices
import PlatoPython
import ctypes
from contextlib import contextmanager
from numpy import *

##############################################################################
## define functionality that redirects console output to a file
##############################################################################
@contextmanager
def redirected(to=os.devnull):
    '''
    import os

    with redirected(to=filename):
        print("from Python")
        os.system("echo non-Python applications are also supported")
    '''
    fdout = sys.stdout.fileno()
    fderr = sys.stderr.fileno()

    def _redirect_stdout(to):
        sys.stdout.close()                  # + implicit flush()
        os.dup2(to.fileno(), fdout)         # fd writes to 'to' file
        sys.stdout = os.fdopen(fdout, 'w')  # Python writes to fd

    def _redirect_stderr(to):
        sys.stderr.close()                  # + implicit flush()
        os.dup2(to.fileno(), fderr)         # fd writes to 'to' file
        sys.stderr = os.fdopen(fderr, 'w')  # Python writes to fd

    with os.fdopen(os.dup(fdout), 'w') as old_stdout, os.fdopen(os.dup(fderr), 'w') as old_stderr:
        with open(to, 'w') as file:
            _redirect_stdout(to=file)
            _redirect_stderr(to=file)
        try:
            yield # allow code to be run with the redirected stdout
        finally:
            _redirect_stdout(to=old_stdout) # restore stdout.
                                            # buffering and flags such as
                                            # CLOEXEC may be different
            _redirect_stderr(to=old_stderr) # restore stderr.
                                            # buffering and flags such as
                                            # CLOEXEC may be different


# boilerplate that dynamically loads the mpi library required by PlatoPython.Analyze
ctypes.CDLL("libmpi.so",mode=ctypes.RTLD_GLOBAL)

# PA and PE require that the mesh exists, so:
os.system("plato-cli geometry esp --input brick.csm --output-model brick_opt.csm --output-mesh brick.exo --tesselation brick.eto")

# create global PlatoServices instance
engineAppFileName = "plato_operations.xml"
engineInputFile = "platomain.xml"
engine = PlatoServices.Services(engineInputFile, engineAppFileName, "services")
engine.initialize()

# create global Analyze instance
analyzeAppFileName = "plato_analyze_operations_1.xml"
analyzeInputFile = "plato_analyze_input_deck_1.xml"

with redirected():
  analyze = PlatoPython.Analyze(analyzeInputFile, analyzeAppFileName, "Inherent Strain")
  analyze.initialize();

#define physics solution
def sol(x):

  with redirected():
    engine.importData("Parameters", "SCALAR", x)
    engine.compute("Update Geometry on Change")

  # import current design into Analyze instance
    analyze.importData("Parameters", "SCALAR", x)
    analyze.compute("Reinitialize on Change")

  # compute cell solutions
    analyze.compute("Compute Displacement Solution")
  
  # export output data from Analyze instance
    solx = analyze.exportData("Solution X", "SCALAR_FIELD")
    soly = analyze.exportData("Solution Y", "SCALAR_FIELD")
    solz = analyze.exportData("Solution Z", "SCALAR_FIELD")
  
    return [solx, soly, solz]


#define objective
def f(x):

  with redirected():
    engine.importData("Parameters", "SCALAR", x)
    engine.compute("Update Geometry on Change")

    # import current design into Analyze instance
    analyze.importData("Parameters", "SCALAR", x)
    analyze.compute("Reinitialize on Change")

    # compute cell solutions
    analyze.compute("Compute Objective Value")
  
    # export output data from Analyze instance
    value = analyze.exportData("Objective Value", "SCALAR")
  
    return value


#define objective
def dfdx(x, grad):

  with redirected():
    engine.importData("Parameters", "SCALAR", x)
    engine.compute("Update Geometry on Change")

    # import current design into Analyze instance
    analyze.importData("Parameters", "SCALAR", x)
    analyze.compute("Reinitialize on Change")

    # compute cell solutions
    analyze.compute("Compute Objective")
  
    # export output data from Analyze instance
    analyzeGrad = analyze.exportData("Objective Gradient", "SCALAR", len(grad))
    value = analyze.exportData("Objective Value", "SCALAR")
  
    for iVal in range(len(analyzeGrad)):
      grad[iVal] = analyzeGrad[iVal]
  
    return value

def gradient_exception_exit_code(err):
  if "HATCHING_GRADIENT" in str(err):
    print("passed")
    return 0
  else:
    print("Expected HATCHING_GRADIENT in error message")
    print("failed")
    return 1

def dfdx_with_gradient_exception_check(xinit, gradP, hatching_gradient_enabled):
  if hatching_gradient_enabled:
    return dfdx(xinit, gradP) # f(x)
  else:
    try:
      dfdx(xinit, gradP)
    except RuntimeError as err:
      print(f"Caught expected exception: {err}")
      sys.exit(gradient_exception_exit_code(err))
    else:
      print("This test is expecting hatching gradients to be disabled, which is detected via an exception.")
      print("An exception is thrown if HATCHING_GRADIENT is disabled in the platoanalyze build, but was not caught.")
      print("If hatching gradients have been fixed, this check can be removed.")
      print("failed")
      sys.exit(1)

xinit = [1.0, 1.0, 1.0, 0.25]
gradP = [0.0 for i in range(len(xinit))]
f_x = dfdx_with_gradient_exception_check(xinit, gradP, hatching_gradient_enabled=False)

g_x = array(gradP) # f'(x)
x = array(xinit)

print( "objective is: ", f_x)
print( "objective gradient is: ", g_x)

#gnorm = linalg.norm(dvdx,2)

h = 0.0001

d = array([0.5, -0.5, 0.5, -0.5]) # a given direction with unit norm
#d = array([1, 0, 0, 0]) # another direction
x_plus = x + h*d

# f(x+h*d) = f(x) + f'(x)*h*d + o(h)

# f(x+h*d)
f_x_plus_dx = dfdx(x_plus.tolist(), gradP)

# (f(x+h*d)-f(x))/h
fd_approx = (f_x_plus_dx - f_x)/h

# f'(x)*d
derivative = dot(g_x,d)

newton_quotient = fd_approx - derivative

print( "step size is: ", h)
print( "perturbed objective is: ", f_x_plus_dx)
print( "finite-difference approximation: ", fd_approx)
print( "derivative: ", derivative)
print( "Newton quotient: ", newton_quotient)

# finalize mpi and kokkos.  Not essential, but you'll get warnings otherwise.
analyze.finalize()
engine.finalize()

if math.fabs(newton_quotient) > 1.0e-3:
  print( "failed")
  sys.exit(1)
else:
  print( "passed")
  sys.exit(0)
