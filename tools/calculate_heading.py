"""
    Calculates heading form two gnss coordinates (WGS84).
"""
import math
import sys
import numpy as np

assert len(sys.argv[1:]) == 4, sys.argv[1:]

try:
    lon1 = float(sys.argv[1])
    lat1 = float(sys.argv[2])
    lon2 = float(sys.argv[3])
    lat2 = float(sys.argv[4])

    heading = np.arctan2(lat2 - lat1, lon2 - lon1)  # Use incorrect heading to east
    print(f"{heading} rad")
    print(f"{math.degrees(heading)} deg")

except ValueError as e:
    print(e)
