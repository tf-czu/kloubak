"""
    Osgar remote control via pygame
"""
import pygame
import math
from threading import Thread


class RcClient:
    def __init__(self, config, bus):
        self.input_thread = Thread(target=self.run_input, daemon=True)
        self.bus = bus
        self.bus.register("desired_speed")
        self.verbose = False
        self.max_spedd = 0.5
        self.max_angular_speed = 1

        pygame.init()
        screen = pygame.display.set_mode((100, 100))


    def start(self):
        self.input_thread.start()

    def join(self, timeout=None):
        self.input_thread.join(timeout=timeout)

    def send_speed(self, speed, angular_speed):
        speed = math.copysign(min(self.max_spedd, abs(speed)), speed)
        angular_speed = math.copysign(min(self.max_angular_speed, abs(angular_speed)), angular_speed)
        self.bus.publish('desired_speed', [round(speed * 1000), round(math.degrees(angular_speed) * 100)])
        print(speed, angular_speed)

        return speed, angular_speed


    def run_input(self):
        speed = 0
        angular_speed = 0

        while self.bus.is_alive():
            self.bus.sleep(0.1)
            events = pygame.event.get()
            for event in events:
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_UP:
                        speed += 0.1
                    elif event.key == pygame.K_DOWN:
                        speed -= 0.1

                    if event.key == pygame.K_LEFT:
                        angular_speed += 0.2
                    elif event.key == pygame.K_RIGHT:
                        angular_speed -= 0.2

                    if event.key == pygame.K_SPACE:
                        speed = 0
                        angular_speed = 0

            speed, angular_speed = self.send_speed(speed, angular_speed)


    def request_stop(self):
        self.bus.shutdown()
