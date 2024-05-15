"""
  Go robot straight
"""

import math

import numpy as np

from osgar.node import Node


class GoStraight(Node):
    def __init__(self, config, bus):
        super().__init__(config, bus)
        bus.register('desired_speed')
        self.max_speed = config.get('max_speed', 0.5)
        self.verbose = False

        self.pose = None
        self.emergency_stop = None

    @staticmethod
    def get_nearest_obstacle(scan):
        scan = np.asarray(scan)
        scan[scan<100] = 20_000
        return np.min(scan), np.argmin(scan)

    def send_speed_cmd(self, speed, angular_speed):
        return self.bus.publish(
            'desired_speed',
            [round(speed*1000), round(math.degrees(angular_speed)*100)]
        )

    def go_safely(self, speed, angular_speed, scan):
        obs_dist, obs_direction = self.get_nearest_obstacle(scan)
        if obs_dist < 500:  # mm
            print("obstackle!")
            speed = 0
        self.send_speed_cmd(speed, angular_speed)

    def on_pose3d(self, data):
        (x, y, z), q = data
        self.pose = [x, y]

    def on_emergency_stop(self, data):
        self.emergency_stop = data

    def on_joint_angle(self, data):
        pass

    def on_position(self, data):
        pass

    def on_scan(self, scan):
        if not self.emergency_stop and self.pose:
            self.go_safely(self.max_speed, 0, scan)

