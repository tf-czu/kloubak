import json
import sys
from matplotlib import pyplot as plt
import numpy as np

def main(annotation_file):
    roads = []
    with open(annotation_file) as f:
        json_data = json.load(f)
        data = json_data["features"]
        for item in data:
            geometry = item["geometry"]
            if geometry["type"] == "LineString":
                road_part = geometry["coordinates"]
                roads.append(road_part)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    for r in roads:
        ar = np.array(r)
        x = ar[:,0]
        y = ar[:,1]
        ax.plot(x,y)
    plt.show()


if __name__ == "__main__":
    main(sys.argv[1])