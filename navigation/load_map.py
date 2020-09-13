import json
import sys
from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
from sklearn.cluster import DBSCAN

import sync_transpositions as tp

fixed_height = 600
start_point = [49.850176,14.1698658,fixed_height]
end_point = [49.8474443,14.1672005,fixed_height]

def print_map(roads,end_point,road_part_interpolated,junctions):
    fig, ax = plt.subplots(figsize=[10, 10], dpi=100, facecolor='w', edgecolor='r')
    for r in roads:
        x = r.north
        y = r.east
        ax.plot(x,y,'og',markersize=5)
    ax.plot(0,0,'+k',markersize=20)
    ax.plot(end_point.north,end_point.east,'+r',markersize=20)
    ax.plot(road_part_interpolated.north,road_part_interpolated.east,'o',markersize=0.5)
    ax.plot(junctions.north,junctions.east,'o',markersize=2)
    ax.set_xlabel('distance to East [m]',size=12)
    ax.set_ylabel('distance to North [m]',size=12)
    ax.set_title('LEGEND: black''+''=start; red''+''=end; green''o''=edge', loc='left')
    ax.minorticks_on()
    ax.tick_params(axis='both',which='major',length=5,width=1,labelsize=10)
    ax.set_aspect('equal', 'datalim')
    ax.grid(True)
    fig.tight_layout()
    # plt.show()
    fig.savefig('map.jpg')

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


def find_sections(road_part):
    sections = []
    for i,road_section in road_part[1:].iterrows():
        start_section = [road_part.north.loc[i-1],road_part.east.loc[i-1]]
        end_section = [road_part.north.loc[i],road_part.east.loc[i]]
        dist = np.sqrt((start_section[0] - end_section[0])**2 + (start_section[1] - end_section[1])**2)
        sections.append(list(start_section + end_section + [dist]))
    return pd.DataFrame(sections,columns=['start_north','start_east','end_north','end_east','dist'])

def interpolate_road_part(sections):
    road_part_interpolated = pd.DataFrame(columns=['east','north'])
    for section_i in range(len(sections)):
        section = pd.DataFrame(columns=['east','north'])
        section.north = np.linspace(sections.start_north[section_i],sections.end_north[section_i],int(sections.dist[section_i] * 2))
        section.east = np.linspace(sections.start_east[section_i],sections.end_east[section_i],int(sections.dist[section_i] * 2))
        road_part_interpolated = road_part_interpolated.append(section)
    return road_part_interpolated

def get_interpolated_roads(roads):
    roads_interpolated = pd.DataFrame(columns=['east','north'])
    for road_i in range(len(roads)):
        sections = find_sections(roads[road_i])
        road_part_interpolated = interpolate_road_part(sections)
        roads_interpolated = roads_interpolated.append(road_part_interpolated)
    return roads_interpolated

def find_junctions(roads_interpolated):
    max_dist = 0.5
    min_points = 3
    junctions = roads_interpolated
    model = DBSCAN(eps=max_dist, min_samples=min_points)
    object_id = model.fit_predict(roads_interpolated)
    junctions["cluster"] = object_id.tolist()
    return junctions[junctions["cluster"] != -1].dropna()

if __name__ == "__main__":
    roads = load_map(sys.argv[1])
    end_point_df = convert_to_enu([[end_point[1],end_point[0]]])

    roads_interpolated = get_interpolated_roads(roads)
    junctions = find_junctions(roads_interpolated)

    print_map(roads,end_point_df,roads_interpolated,junctions)
