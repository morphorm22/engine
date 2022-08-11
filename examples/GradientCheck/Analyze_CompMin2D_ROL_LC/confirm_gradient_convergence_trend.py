import sys
import numpy as np

numInitialPoints = sys.argv[1]
tolerance = sys.argv[2]
testTextFile = sys.argv[3]
goldTextFile = sys.argv[4]

testData = np.loadtxt(testTextFile, usecols=(4), skiprows=int(numInitialPoints)+1, dtype='double')
goldData = np.loadtxt(goldTextFile, usecols=(4), skiprows=int(numInitialPoints)+1, dtype='double')


print(testData)
print(goldData)

#testDataSorted = testData[np.argsort(testData[:,1]),:]
#goldDataSorted = goldData[np.argsort(goldData[:,1]),:]

#relativeErrors = []
#for iVal in range(0,testDataSorted.shape[0]):
#    if ( (testDataSorted[iVal,1] > goldDataSorted[0,1]) and (testDataSorted[iVal,1] < goldDataSorted[-1,1]) ):
#        idx = np.argmax(goldDataSorted[:,1] > testDataSorted[iVal,1])
#        interp = np.interp(testDataSorted[iVal,1], goldDataSorted[idx-1:idx+1,1], goldDataSorted[idx-1:idx+1,0])
#        relativeErrors.append(np.abs(testDataSorted[iVal,0] - interp)/interp)

#infinityNorm = np.linalg.norm(relativeErrors,ord=np.inf)
#if (infinityNorm < float(tolerance)):
#    sys.exit(0)
#else:
    sys.exit(1)

