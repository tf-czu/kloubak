"""
  Go robot straight
"""
import math

import numpy as np

from osgar.node import Node
from osgar.lib.quaternion import heading
from app.rc_client import angle_angular_speed


def get_diff_angle(phi_0, phi):
    diff = phi_0 - phi
    adjusted_diff = phi_0 - phi + math.copysign(360, -diff)
    if abs(diff) <= abs(adjusted_diff):
        return diff
    return adjusted_diff


class GoStraight(Node):
    def __init__(self, config, bus):
        super().__init__(config, bus)
        bus.register('desired_speed')
        self.max_speed = config.get('max_speed', 0.5)
        self.desired_heading = config['desired_heading']
        print(f"Desired direction: {self.desired_heading}")

        self.verbose = False
        self.pose = None
        self.emergency_stop = None
        self.scan = None

    @staticmethod
    def get_nearest_obstacle(scan):
        scan = np.asarray(scan)
        scan[scan<100] = 20_000
        return np.min(scan), np.argmin(scan)

    def send_speed_cmd(self, speed, angular_speed):
        return self.bus.publish(
            'desired_speed',
            [round(speed*1000), round(math.degrees(angular_speed))*100]
        )

    def go_safely(self, speed, angle, scan):
        obs_dist, obs_direction = self.get_nearest_obstacle(scan)

        if obs_dist > 500:  # mm
            # influencing obstacle 0.5 - 1.0 m
            speed = max(0.1, speed * min(1, (obs_dist - 500) / 500))
        else:
            print("Obstacle!")
            speed = 0
        if self.verbose:
            print(f"Obstacle dist: {obs_dist} mm, speed: {speed} m/s")

        angular_speed = angle_angular_speed(speed, angle)
        self.send_speed_cmd(speed, angular_speed)

    def on_pose3d(self, data):
        (x, y, z), q = data
        self.pose = [x, y]
        last_heading = math.degrees(heading(q))
        if self.verbose:
            print(f"last heading: {last_heading}")

        if not self.emergency_stop:
            direction_diff_deg = get_diff_angle(self.desired_heading, last_heading)
            if self.verbose:
                print(f"direction diff: {direction_diff_deg}")

            # self.go_safely(self.max_speed, direction_diff_deg, scan)  # TODO fix lidar
            angular_speed = angle_angular_speed(self.max_speed, direction_diff_deg)
            self.send_speed_cmd(self.max_speed, angular_speed)



    def on_emergency_stop(self, data):
        self.emergency_stop = data


    def on_scan(self, scan):
        self.scan = scan
