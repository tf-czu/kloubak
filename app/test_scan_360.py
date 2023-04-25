import unittest
from unittest.mock import MagicMock

import numpy as np

from app.scan360 import Scan360

class TestScan360(unittest.TestCase):
    def test_combine_scans(self):
        bus = MagicMock()
        c = Scan360(bus=bus.handle(), config={})
        c.scan_front = np.ones(811, dtype=np.int32)
        c.scan_rear = np.ones(811, dtype=np.int32)
        c.joint_angle = 0
        scan_360 = c.combine_scans()
        expected_res = np.ones(1081, dtype=np.int32)
        self.assertTrue(np.array_equal(scan_360, expected_res))

        c.scan_front = c.scan_front*2
        expected_res2 = expected_res
        expected_res2[3*135: 3 * 225] = 2
        scan_360_2 = c.combine_scans()
        self.assertTrue(np.array_equal(scan_360_2, expected_res2))

    def test_scan_offset(self):
        bus = MagicMock()
        c = Scan360(bus=bus.handle(), config={})
        c.scan_front = np.ones(811, dtype=np.int32)
        c.scan_rear = np.zeros(811, dtype=np.int32)
        c.joint_angle = -90
        scan_360 = c.combine_scans()
        expected_res = np.zeros(1081, dtype=np.int32)
        expected_res[:811] = 1
        self.assertTrue(np.array_equal(scan_360, expected_res))

        c.joint_angle = 90
        scan_360_2 = c.combine_scans()
        expected_res2 = np.zeros(1081, dtype=np.int32)
        expected_res2[3*90:] = 1
        self.assertTrue(np.array_equal(scan_360_2, expected_res2))

    def test_scan_offset_rear(self):
        bus = MagicMock()
        c = Scan360(bus=bus.handle(), config={})
        c.scan_front = np.zeros(811, dtype=np.int32)
        c.scan_rear = np.ones(811, dtype=np.int32)
        c.joint_angle = -90
        scan_360 = c.combine_scans()
        expected_res = np.zeros(1081, dtype=np.int32)
        expected_res[:3*180] = 1
        expected_res[-3*90-1:] = 1
        self.assertTrue(np.array_equal(scan_360, expected_res))

if __name__ == '__main__':
    unittest.main()
