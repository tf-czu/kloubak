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
        self.joint_angle = None  # angle in degrees
        self.move_backward = False
        self.scan_front = None
        self.scan_rear = None

    def combine_scans(self):
        if self.scan_front is None or self.scan_rear is None or self.joint_angle is None:
            return
        assert len(self.scan_front) == 811, len(self.scan_front)
        assert len(self.scan_rear) == 811, len(self.scan_rear)
        angle_offset = int(round(self.joint_angle*3/2))  # angle resolution is 1/3
        scan_360 = np.zeros(1081, dtype=np.int32)
        scan_360_rear = scan_360.copy()

        # The scan start is at the back of the robot.
        scan_360[3 * 45 + angle_offset : 3 * 315 + 1 + angle_offset] = self.scan_front  # The scan length is 3*270+1.
        scan_360_rear[3 * 225 - angle_offset:] = self.scan_rear[:3 * 135 + 1 + angle_offset]
        scan_360_rear[:3*135 - angle_offset] = self.scan_rear[3 * 135 + 1 + angle_offset:]
        mask = np.logical_and(scan_360_rear != 0, np.logical_or(scan_360_rear < scan_360, scan_360 == 0))
        scan_360[mask] = scan_360_rear[mask]

        return scan_360

    def process_scans(self):
        scan_360 = self.combine_scans()
        if scan_360 is not None:
            self.publish("scan360", scan_360)

    def on_desired_speed(self, data):
        speed, angular_speed = data
        if speed < 0:
            self.move_backward = True
        else:
            self.move_backward = False

    def on_scan_rear(self, data):
        self.scan_rear = data
        if self.move_backward:
            self.process_scans()

    def on_scan_front(self, data):
        self.scan_front = data
        if not self.move_backward:
            self.process_scans()

    def on_joint_angle(self, data):
        self.joint_angle = data[0]/100
