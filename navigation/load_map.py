# =============================================================================
# Navigation planner
# =============================================================================
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
    ax.plot(junctions.north,junctions.east,'oy',markersize=2)
    ax.set_xlabel('distance to East [m]',size=12)
    ax.set_ylabel('distance to North [m]',size=12)
    ax.set_title('LEGEND: black ''+''=start; red ''+''=end; green ''o''=edge; yelow ''o''=junction', loc='left')
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

def interpolate_road_part(sections,road_i,section_i_last):
    road_part_interpolated = pd.DataFrame(columns=['east','north','road','section'])
    for section_i in range(len(sections)):
        section = pd.DataFrame(columns=['east','north','road','section'])
        section.north = np.linspace(sections.start_north[section_i],sections.end_north[section_i],int(sections.dist[section_i] * 2))
        section.east = np.linspace(sections.start_east[section_i],sections.end_east[section_i],int(sections.dist[section_i] * 2))
        section.road = road_i
        section.section = section_i + section_i_last + 1
        road_part_interpolated = road_part_interpolated.append(section)
    return road_part_interpolated,section_i + section_i_last + 1

def get_interpolated_roads(roads):
    roads_interpolated = pd.DataFrame(columns=['east','north','road','section'])
    section_i = 0
    for road_i in range(len(roads)):
        sections = find_sections(roads[road_i])
        road_part_interpolated,section_i = interpolate_road_part(sections,road_i,section_i)
        roads_interpolated = roads_interpolated.append(road_part_interpolated)
    return roads_interpolated

def get_new_roads(roads_interpolated):
    roads_grouped = list(roads_interpolated.groupby(['section']))
    roads_info_list = []
    for road_i in range(len(roads_grouped)):
        start_north = roads_grouped[road_i][1].iloc[0].north
        start_east = roads_grouped[road_i][1].iloc[0].east
        start_cluster = roads_grouped[road_i][1].iloc[0].cluster
        end_north = roads_grouped[road_i][1].iloc[max(roads_grouped[road_i][1].index)].north
        end_east = roads_grouped[road_i][1].iloc[max(roads_grouped[road_i][1].index)].east
        end_cluster = roads_grouped[road_i][1].iloc[max(roads_grouped[road_i][1].index)].cluster
        dist = len(roads_grouped[road_i][1])
        roads_info_list.append([start_north,start_east,start_cluster,road_i,dist])
        roads_info_list.append([end_north,end_east,end_cluster,road_i,dist])
    round_bounds = pd.DataFrame(roads_info_list,columns=['north','east','cluster','road','dist'])
    return roads_grouped,round_bounds

def group_junctions(junctions):
    junctions_grouped = []
    for cluster_i in range(junctions.cluster.max()):
        cluster = junctions[junctions.cluster == cluster_i]
        sections_list = cluster.section.values.tolist()
        roads_list = cluster.road.values.tolist()
        cluster_mean = cluster.mean()
        junctions_grouped.append([cluster_mean,roads_list,sections_list])
    return junctions_grouped

def find_junctions(roads_interpolated):
    max_dist = 0.5
    min_points = 3
    junctions = roads_interpolated
    model = DBSCAN(eps=max_dist, min_samples=min_points)
    object_id = model.fit_predict(roads_interpolated[['east','north']])
    junctions["cluster"] = object_id.tolist()
    return junctions[junctions["cluster"] != -1].dropna()#.drop_duplicates(subset='cluster', keep="last")

def find_neighbors(round_bounds):
    bounds_matrix = np.ones([len(round_bounds),len(round_bounds)])#,2])
    for bound_iy in range(len(round_bounds)):
        for bound_ix in range(bound_iy + 1,len(round_bounds)):
            north_diff = round_bounds.iloc[bound_ix].north - round_bounds.iloc[bound_iy].north
            east_diff = round_bounds.iloc[bound_ix].east - round_bounds.iloc[bound_iy].east
            distance = np.sqrt((north_diff)**2 + (east_diff)**2)
            bounds_matrix[bound_ix,bound_iy] = distance
    return np.where(bounds_matrix < 0.5)

if __name__ == "__main__":
    roads = load_map(sys.argv[1])
    end_point_df = convert_to_enu([[end_point[1],end_point[0]]])

    roads_interpolated = get_interpolated_roads(roads)

    junctions = find_junctions(roads_interpolated)
    junctions_grouped = group_junctions(junctions)

    roads_grouped,round_bounds = get_new_roads(roads_interpolated)

    neighbors = find_neighbors(round_bounds)


    print_map(roads,end_point_df,roads_interpolated,junctions)
