import sys
import numpy as np
import filecmp
import subprocess

def readFile(filename):
     with open(filename) as fileNames:
          testFiles = fileNames.read().splitlines()
     return testFiles

def compareListsOfFiles(listOne, numdiff=False, tolerance=1e-3 ):
     length = len(listOne)
     for i in range(length):
          print("Comparing ",listOne[i],"with gold/"+listOne[i])
          goodComparison = False
          try:
               if( numdiff == False ):
                    goodComparison = filecmp.cmp(listOne[i],"gold/"+listOne[i])
               else:
                    tolcom = "--relative-tolerance="+tolerance
                    returnval = subprocess.run(["numdiff",tolcom,listOne[i],"gold/"+listOne[i] ],stdout=subprocess.PIPE)
                    goodComparison = (returnval.returncode == 0)
          except FileNotFoundError:
               print("One of these files not found: ",listOne[i],"gold/"+listOne[i])
               return False
          print("Comparison complete, equality found:",goodComparison)
          if goodComparison != True:
               if( numdiff == False ):
                    returnval = subprocess.run(["diff",listOne[i],"gold/"+listOne[i] ],stdout=subprocess.PIPE)
                    print(returnval.stdout.decode("utf-8"))
               else:
                    print(returnval.stdout.decode("utf-8"))
               return False
     return True

tolerance = sys.argv[1]

testFiles = readFile(sys.argv[2])
print("XML Generation List of Files to Compare: ",testFiles)
fileComparisonResult = compareListsOfFiles(testFiles)
if (fileComparisonResult == False):
    sys.exit(1)

testFiles = readFile(sys.argv[3])
print("Serialized Data List of Files to Compare: ",testFiles)
fileComparisonResult = compareListsOfFiles(testFiles)
if (fileComparisonResult == False):
    sys.exit(1)

testFiles = readFile(sys.argv[4])
print("Numdiff Data List of Files to Compare: ",testFiles)
fileComparisonResult = compareListsOfFiles(testFiles, True,tolerance)
if (fileComparisonResult == True):
    sys.exit(0)
else:
    sys.exit(1)

