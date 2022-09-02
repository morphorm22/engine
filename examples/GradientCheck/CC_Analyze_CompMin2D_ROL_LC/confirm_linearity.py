import sys
import numpy as np

tolerance = sys.argv[1]
err = 0
with open('ROL_linearity_check_output.txt','r') as fp:
    for line in fp:
        try:
            err = np.abs(float(line.strip().split("=")[1]))
        except (IndexError,ValueError):
            continue
print(err)
if (err < float(tolerance)):
    sys.exit(0)
else:
    sys.exit(1)
