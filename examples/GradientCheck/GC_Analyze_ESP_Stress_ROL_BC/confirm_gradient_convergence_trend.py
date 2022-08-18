import sys
import numpy as np

numInitialPoints = sys.argv[1]
tolerance = sys.argv[2]
testTextFile = sys.argv[3]
goldTextFile = sys.argv[4]
stopRow = int(sys.argv[5])-1

testData = np.loadtxt(testTextFile, usecols=(3), skiprows=int(numInitialPoints)+1, dtype='double')
goldData = np.loadtxt(goldTextFile, usecols=(3), skiprows=int(numInitialPoints)+1, dtype='double')

relativeErrors = []
for iVal in range(0,stopRow):
     relativeErrors.append(np.abs(np.log10(testData[iVal]) - np.log10(goldData[iVal]))/np.log10(goldData[iVal]))

print(relativeErrors)

infinityNorm = np.linalg.norm(relativeErrors,ord=np.inf)

print(infinityNorm)

if (infinityNorm < float(tolerance)):
    sys.exit(0)
else:
    sys.exit(1)

