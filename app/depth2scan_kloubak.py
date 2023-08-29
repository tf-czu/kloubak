"""
  Convert and adjust depth image to "lidar" scan on Kloubak robot
"""
import math

from osgar.node import Node
from osgar.lib.depth import depth2dist, DepthParams
from subt.depth2scan import adjust_scan


class DepthToScan(Node):
    def __init__(self, config, bus):
        super().__init__(config, bus)
        bus.register("scan")
        self.depth = None  # initialize inputs
        self.last_scan = None
        self.verbose = False
        self.pitch = math.radians(config.get("pitch", 0))
        self.depth_params = DepthParams(**config.get('depth_params', {}))

    def on_scan(self, data):
        self.last_scan = data

    def on_depth(self, data):
        depth = data/1000
        depth_scan = depth2dist(depth, self.depth_params, pitch = self.pitch)
        if self.last_scan is not None:
            scan = self.last_scan
            new_scan = adjust_scan(scan, depth_scan, self.depth_params)
            self.publish('scan', new_scan.tolist())
            self.last_scan = None
