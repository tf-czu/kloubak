import unittest
from app.go_straight import get_diff_angle

class TestGoStraight(unittest.TestCase):
    def test_get_diff_angle(self):
        phi_0 = [20, 20, 20, 350, 350]
        phi = [30, 10, 350, 10, 340]
        expected_diff = [-10, 10, 30, -20, 10]
        for p_0, p, e_diff in zip(phi_0, phi, expected_diff):
            diff = get_diff_angle(p_0, p)
            self.assertEqual(diff, e_diff)
