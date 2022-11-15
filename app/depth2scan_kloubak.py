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
        self.scan = None
        self.verbose = False
        self.pitch = math.radians(-20)
        self.depth_params = DepthParams(**config.get('depth_params', {}))

    def update(self):
        channel = super().update()
        assert channel in ["scan", "depth"], channel  # TODO pose3D

        if channel == 'depth' and self.scan is not None:
            depth = self.depth/1000
            scan = self.scan
            depth_scan = depth2dist(depth, self.depth_params, pitch = self.pitch)
            new_scan = adjust_scan(scan, depth_scan, self.depth_params)
            self.publish('scan', new_scan.tolist())

        return channel
