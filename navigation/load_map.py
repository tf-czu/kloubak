import json
import sys
from matplotlib import pyplot as plt
import numpy as np
import pandas as pd

import sync_transpositions as tp

fixed_height = 600
start_point = [49.850176,14.1698658,fixed_height]
end_point = [49.8474443,14.1672005,fixed_height]

def load_map(annotation_file):
    roads = []
    with open(annotation_file) as f:
        json_data = json.load(f)
        data = json_data["features"]
        for item in data:
            geometry = item["geometry"]
            if geometry["type"] == "LineString":
                road_part = geometry["coordinates"]
                enu_df = convert_to_enu(road_part)
                roads.append(enu_df)
    return roads

def convert_to_enu(wgs_points):
    wgs_points_df = pd.DataFrame(wgs_points,columns=['lat','long'])
    wgs_points_df['lat_rad'] = wgs_points_df.lat * np.pi / 180
    wgs_points_df['long_rad'] = wgs_points_df.long * np.pi / 180
    wgs_points_df['height'] = fixed_height
    xyz = tp.wgs2xyz(wgs_points_df[['long_rad','lat_rad','height']].values)
    enu = tp.xyz2enu(xyz,start_point)
    return pd.DataFrame(enu,columns=['north','east','up'])

def print_map(roads,end_point):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    for r in roads:
        x = r.north
        y = r.east
        ax.plot(x,y)
    ax.plot(0,0,'+k',markersize=20)
    ax.plot(end_point.north,end_point.east,'+r',markersize=20)
    ax.set_xlabel('distance to East [m]',size=12)
    ax.set_ylabel('distance to North [m]',size=12)
    ax.set_title('Map: black=start; red=end', loc='left')
    ax.minorticks_on()
    ax.tick_params(axis='both',which='major',length=5,width=1,labelsize=10)
    ax.set_aspect('equal', 'datalim')
    ax.grid(True)
    fig.tight_layout()
    plt.show()

if __name__ == "__main__":
    roads = load_map(sys.argv[1])

    end_point_df = convert_to_enu([[end_point[1],end_point[0]]])

    print_map(roads,end_point_df)
