import sys
import os
import exodus
import PythonPlatoESP

# specify sensitivity tolerance
sensitivityTol = float(sys.argv[1])

# specify geometry name and parameter values
model = "cuboid"
Lx = 3.0
Ly = 2.0
Lz = 4.0

# specify plato ESP performer xml files
performerInput = "plato_esp_input_deck.xml"
performerOperations = "plato_esp_operations.xml"

# use esp to create mesh
espCommand = "plato-cli geometry esp"
espCommand += " --input " + model + ".csm"
espCommand += " --output-model " + model + "_opt.csm"
espCommand += " --output-mesh " + model + ".exo"
espCommand += " --tesselation " + model + ".eto"
espCommand += " --parameters " + str(Lx) + " " +  str(Ly) + " " + str(Lz)
os.system(espCommand)

# read in mesh
mesh = exodus.ExodusDB()
mesh.read(model + ".exo")

# create plato esp instances
platoESP0 = PythonPlatoESP.PlatoESP(performerInput, performerOperations, "plato esp instance")
platoESP0.initialize()
platoESP0.importData("Parameter Index", "SCALAR PARAMETER", [0.0])

platoESP1 = PythonPlatoESP.PlatoESP(performerInput, performerOperations, "plato esp instance")
platoESP1.initialize()
platoESP1.importData("Parameter Index", "SCALAR PARAMETER", [1.0])

platoESP2 = PythonPlatoESP.PlatoESP(performerInput, performerOperations, "plato esp instance")
platoESP2.initialize()
platoESP2.importData("Parameter Index", "SCALAR PARAMETER", [2.0])

# compute sensitivity
platoESP0.compute("Compute Parameter Sensitivity On Change")
DXDp0 = platoESP0.exportData("Parameter Sensitivity", "SCALAR")

platoESP1.compute("Compute Parameter Sensitivity On Change")
DXDp1 = platoESP1.exportData("Parameter Sensitivity", "SCALAR")

platoESP2.compute("Compute Parameter Sensitivity On Change")
DXDp2 = platoESP2.exportData("Parameter Sensitivity", "SCALAR")

# function to check sensitivity values on face
def check_face_sensitivity(dim, faceCoord, grad, gold):

    coordTol = 1.0E-8 * abs(2.0*faceCoord)

    errors = []
    for nodeIndex in range(mesh.numNodes):
        X = mesh.getCoordData(nodeIndex, "index")
        if (X[dim] <= (faceCoord + coordTol) and X[dim] >= (faceCoord - coordTol)):
            relError = (grad[3*nodeIndex + dim] - gold) / gold
            errors.append(abs(relError))

    return errors

# check sensitivity values for nodes on x+ surface
DXDp_gold = 0.5 # x(p) = p/2 for nodes on x+ since p = Lx
errorsXP = check_face_sensitivity(0, Lx/2.0, DXDp0, DXDp_gold)
maxErrorXP = max(errorsXP)

# check sensitivity values for nodes on x- surface
DXDp_gold = -0.5 # x(p) = -p/2 for nodes on x- since p = Lx
errorsXM = check_face_sensitivity(0, -Lx/2.0, DXDp0, DXDp_gold)
maxErrorXM = max(errorsXM)

# check sensitivity values for nodes on y+ surface
DXDp_gold = 0.5 # y(p) = p/2 for nodes on y+ since p = Ly
errorsYP = check_face_sensitivity(1, Ly/2.0, DXDp1, DXDp_gold)
maxErrorYP = max(errorsYP)

# check sensitivity values for nodes on y- surface
DXDp_gold = -0.5 # y(p) = -p/2 for nodes on y- since p = Ly
errorsYM = check_face_sensitivity(1, -Ly/2.0, DXDp1, DXDp_gold)
maxErrorYM = max(errorsYM)

# check sensitivity values for nodes on z+ surface
DXDp_gold = 0.5 # z(p) = p/2 for nodes on z+ since p = Lz
errorsZP = check_face_sensitivity(2, Lz/2.0, DXDp2, DXDp_gold)
maxErrorZP = max(errorsZP)

# check sensitivity values for nodes on z- surface
DXDp_gold = -0.5 # z(p) = -p/2 for nodes on z- since p = Lz
errorsZM = check_face_sensitivity(2, -Lz/2.0, DXDp2, DXDp_gold)
maxErrorZM = max(errorsZM)

# write errors for posterity
def write_errors(outFile, errors):
    outFile.write("\n error values: [ ")
    for error in errors:
        outFile.write("%3.4E " % (error))
    outFile.write(" ] \n")

outFile = open("errors.out","w")
outFile.write("x+ errors: " )
write_errors(outFile, errorsXP)
outFile.write("x- errors: " )
write_errors(outFile, errorsXM)
outFile.write("y+ errors: " )
write_errors(outFile, errorsYP)
outFile.write("y- errors: " )
write_errors(outFile, errorsYM)
outFile.write("z+ errors: " )
write_errors(outFile, errorsZP)
outFile.write("z- errors: " )
write_errors(outFile, errorsZM)
outFile.close()

# check errors
errorsAcceptable = 1
errorArray = (maxErrorXP, maxErrorXM, maxErrorYP, maxErrorYM, maxErrorZP, maxErrorZM)
for error in errorArray:
    if(error > sensitivityTol):
        errorsAcceptable = 0
    
if(errorsAcceptable):
    print("\n Success: Sensitivity values match gold")
    sys.exit(0)
else:
    print("\n Failure: Sensitivity values do not match gold")
    sys.exit(1)
