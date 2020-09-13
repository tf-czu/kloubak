# git live here
# -*- coding: utf-8 -*-
"""
Created on Fri Aug  9 15:02:18 2019

@author: xkadj
"""
import numpy as np

def get_seconds(utc):
    seconds = float(utc[0:2])*3600 + float(utc[2:4])*60 + float(utc[4:])
    return seconds

def get_coordinate(coor):
    coor = float(coor)
    coor = coor/100
    coorDgr = int(coor)
    coorMin = (coor-coorDgr)/60*100
    coor = coorDgr + coorMin
    return coor

def wgs2xyz(wgs):
    '''
    conversion from wgs to xyz
    :param wgs: [lat lon h], lat and lon in radians
    :return: xyz [x y z]
    '''
    a = 6378137
    e2 = 0.006694379987901

    xyz = np.zeros((len(wgs),3))

    dnm = np.sqrt(1 + (1 - e2) * np.tan(wgs[:,0])**2)
    xyz[:,0] = a * np.cos(wgs[:,1])/ dnm + wgs[:,2]*np.cos(wgs[:,1])*np.cos(wgs[:,0])
    xyz[:,1] = a * np.sin(wgs[:,1])/ dnm + wgs[:,2]*np.sin(wgs[:,1])*np.cos(wgs[:,0])
    xyz[:,2] = a * (1 - e2) * np.sin(wgs[:,0])/ np.sqrt(1 - e2 * np.sin(wgs[:,0])**2)+wgs[:,2]*np.sin(wgs[:,0])

    return xyz

def xyz2enu(xyz,wgs_ref,method = 'def'):

    wgs_ref = np.array([wgs_ref])
    wgs_ref[0,[0,1]] = wgs_ref[0,[0,1]]*np.pi/180

    if method == 'ECEF':
        xyz_ref = wgs2ECEF(wgs_ref)
    else:
        xyz_ref = wgs2xyz(wgs_ref)

    xyz[:,0] = xyz[:,0] - xyz_ref[0,0]
    xyz[:,1] = xyz[:,1] - xyz_ref[0,1]
    xyz[:,2] = xyz[:,2] - xyz_ref[0,2]

    R = np.array([[-np.sin(wgs_ref[0,1]), np.cos(wgs_ref[0,1]), 0],
                  [-np.cos(wgs_ref[0,1])*np.sin(wgs_ref[0,0]), -np.sin(wgs_ref[0,1])*np.sin(wgs_ref[0,0]),np.cos(wgs_ref[0,0])],
                  [np.cos(wgs_ref[0,1])*np.cos(wgs_ref[0,0]), np.sin(wgs_ref[0,1])*np.cos(wgs_ref[0,0]),np.sin(wgs_ref[0,0])]])
    enu = np.dot(xyz,R.T)
    enu.T[1] = enu.T[1]*(-1)
    return enu

def wgs2ECEF(wgs):
    a = 6378137
    b = 6.356752314252350e+06
    e2 = 0.006694379987901
    xyz = np.zeros((len(wgs), 3))


    N = a/(np.sqrt(1 - e2*(np.sin(wgs[:,0]))**2))
    xyz[:,0] = (N + wgs[:,2]) * np.cos(wgs[:,0]) * np.cos(wgs[:,1])
    xyz[:,1] = (N + wgs[:,2]) * np.cos(wgs[:,0]) * np.sin(wgs[:,1])
    xyz[:,2] = (((b/a)**2)*N + wgs[:,2]) * np.sin(wgs[:,0])

    return xyz

def wgs2enu(wgs,wgs_ref):
    xyz_ref = wgs2ECEF(wgs_ref)
    xyz = wgs2ECEF(wgs)

    xyz[:, 0] = xyz[:, 0] - xyz_ref[0, 0]
    xyz[:, 1] = xyz[:, 1] - xyz_ref[0, 1]
    xyz[:, 2] = xyz[:, 2] - xyz_ref[0, 2]

    R = np.array([[-np.sin(wgs_ref[0, 1]), np.cos(wgs_ref[0, 1]), 0],
                  [-np.cos(wgs_ref[0, 1]) * np.sin(wgs_ref[0, 0]), -np.sin(wgs_ref[0, 1]) * np.sin(wgs_ref[0, 0]),
                   np.cos(wgs_ref[0, 0])],
                  [np.cos(wgs_ref[0, 1]) * np.cos(wgs_ref[0, 0]), np.sin(wgs_ref[0, 1]) * np.cos(wgs_ref[0, 0]),
                   np.sin(wgs_ref[0, 0])]])
    enu = np.dot(xyz, R.T)

    return enu

def bearing(wgs_1,wgs_2):
    lat1 = wgs_1[0,0]
    lat2 = wgs_2[0,0]
    lon1 = wgs_1[0,1]
    lon2 = wgs_2[0,1]
    dlon = np.deg2rad(lon2 - lon1)

    bearing = np.arctan((np.sin(dlon) * np.cos(np.deg2rad(lat2))) / (np.cos(np.deg2rad(lat1)) * np.sin(np.deg2rad(lat2)) - np.sin(np.deg2rad(lat1)) * np.cos(np.deg2rad(lat2)) * np.cos(dlon)))
    return 360 + bearing * 180 / np.pi

    # bearing = np.arctan((np.sin(dlon) * np.cos(np.deg2rad(lat2))) / (
    # np.cos(np.deg2rad(lat1)) * np.sin(np.deg2rad(lat2)) - np.sin(np.deg2rad(lat1)) * np.cos(np.deg2rad(lat2)) * np.cos(
    #     dlon)))
    # return 360 + (bearing * 180 / np.pi)
