"""
    Pose viewer and export
"""
import os.path
import csv

from matplotlib import pyplot as plt

from osgar.logger import LogReader, lookup_stream_id
from osgar.lib.serialize import deserialize
from osgar.lib.route import Convertor

# sys.path.append("../spider/")
# from lib.localization import Localization


def list2xy(data):
    x = [coord[0] for coord in data]
    y = [coord[1] for coord in data]
    return x, y


def save_images(images, video_name):
    with open(video_name, "wb") as f:
        for im in images:
            f.write(im)


def save_csv(data, csv_filename):
    with open(csv_filename, "w", newline='') as csv_file:
        csv_writer = csv.writer(csv_file, delimiter=",")
        csv_writer.writerow(["micros", "latitude", "longitude"])
        for lat, lon, time_id in data:
             csv_writer.writerow([time_id, lat, lon])


def export_data(log_file, latlon, img_list, img_rs_list):
    print(len(latlon), len(img_list), len(img_rs_list))
    dir_name = os.path.basename(log_file).split(".")[0]
    im_name = os.path.join(dir_name, "arecont.mjpeg")
    rs_name = os.path.join(dir_name, "rs_camera.mjpeg")
    os.makedirs(dir_name, exist_ok=False)
    save_images(img_list, im_name)
    save_images(img_rs_list, rs_name)
    save_csv(latlon, os.path.join(dir_name, "latlon_data.csv"))


def load_data(log_file, nmea_streamm, start, end):
    pose_data = []
    # pose_kalman = []
    img_list = []
    img_rs_list = []
    latlon = []
    convertor = None
    # localization = Localization()
    only_nmea = lookup_stream_id(log_file, nmea_streamm)
    only_im = lookup_stream_id(log_file, "camera.raw")
    try:
        only_rs = lookup_stream_id(log_file, "realsense.color")
    except:
        only_rs = None
    with LogReader(log_file, only_stream_id=[only_nmea, only_im, only_rs]) as log:
        for timestamp, stream_id, data in log:
            if timestamp.total_seconds() < start:
                continue
            if timestamp.total_seconds() > end:
                break
            data = deserialize(data)
            if stream_id == only_nmea:
                if nmea_streamm == "gps.position":
                    lon_ms, lat_ms = data
                    lat = lat_ms/3_600_000
                    lon = lon_ms/3_600_000
                else:
                    lon = data["lon"]
                    lat = data["lat"]
                    assert data["lon_dir"] == "E"
                    assert data["lat_dir"] == "N"
                    assert data["quality"] == 1, data["quality"]
                if convertor:
                    x, y = convertor.geo2planar((lon, lat))
                else:
                    convertor = Convertor((lon, lat))
                    x, y = 0, 0
                pose_data.append([x, y, timestamp])
                # print(timestamp, int(timestamp.total_seconds()*1_000_000))
                # localization.update_xyz_from_gps(timestamp, [x, y, 0], gps_err=[5, 5, 15])
                # kalman_pose3d = localization.get_pose3d()  # get last position from kalman
                # if kalman_pose3d:
                #     xyz, __ = kalman_pose3d
                #     pose_kalman.append([xyz[0], xyz[1]])

                latlon.append([lat, lon, int(timestamp.total_seconds() * 1_000_000)])

            if stream_id == only_im:
                img_list.append(data)
            if stream_id == only_rs:
                img_rs_list.append(data)

    return pose_data, latlon, img_list, img_rs_list


def draw_pose(pose_data):
    x, y = list2xy(pose_data)
    plt.plot(x, y, "k+-", label="org_gps")
    plt.legend()
    plt.axis('equal')
    plt.show()


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('logfile', help='path to logfile')
    parser.add_argument('--nmea', help='nmea stream', default='gps.nmea_data')
    parser.add_argument('--start', help='start time', default=0, type=float)
    parser.add_argument('--end', help='end time', default=10_000, type=float)
    parser.add_argument('--draw', help='Draw gps positions', action='store_true')

    args = parser.parse_args()

    pose_data, latlon, img_list, img_rs_list = load_data(args.logfile, args.nmea, args.start, args.end)
    if args.draw:
        draw_pose(pose_data)
    export_data(args.logfile, latlon, img_list, img_rs_list)
