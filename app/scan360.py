"""
    Combines two Kloubak 270 scans.
"""

import numpy as np
import math
from osgar.node import Node


class Scan360(Node):
    def __init__(self, config, bus):
        super().__init__(config, bus)
        bus.register("scan360")
        self.bus = bus
        self.verbose = False

        self.joint_angle = None
        self.move_backward = False
        self.scan_front = None

    def combine_scans(self):
        pass

    def on_desired_spped(self, data):
        speed, angular_speed = data
        if speed < 0:
            self.move_backward = True
        else:
            self.move_backward = False

    def on_scan_rear(self, data):
        pass

    def on_scan_front(self, data):
        pass

    def on_joint_angle(self, data):
        self.joint_angle = math.radians(data/100)
