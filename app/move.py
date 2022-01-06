"""
  Sample application for robot Kloubak. Just move forward.
"""
from datetime import timedelta
import math

from osgar.node import Node


class Move(Node):
    def __init__(self, config, bus):
        super().__init__(config, bus)
        bus.register('desired_speed')
        self.verbose = False

    def update(self):
        channel = super().update() # define self.time
        if self.verbose:
            print(channel)
        # if channel == "pose2d":
        #     print(self.pose2d)
        # if channel == "scan":
        #     print(len(self.scan))


    def send_speed_cmd(self, speed, angular_speed):
        return self.publish('desired_speed', [round(speed*1000), round(math.degrees(angular_speed)*100)])

    def wait(self, dt):
        dt = timedelta(seconds=dt)
        if self.time is None:
            self.update()
        start_time = self.time
        while self.time - start_time < dt:
            self.update()

    def run(self):
        print("Move!")
        self.send_speed_cmd(0.5, 0.0)
        self.wait(5)
        self.send_speed_cmd(0, 0)
        print("END")


if __name__ == "__main__":
    from osgar.launcher import launch

    launch(app=Move, description='Just move forward', prefix='move-')