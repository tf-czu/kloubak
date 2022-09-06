"""
    Osgar launcher
"""

import subprocess
import signal
import contextlib
import zmq
import time
import osgar.lib.serialize

class OsgarLaunch:
    def __init__(self):
        self.command = ["python3", "-m", "osgar.record"]
        self.shell = False  # or True?? is it needed?

    def start(self, config_file):
        self.command.append(config_file)
        self.running = subprocess.Popen(self.command, shell=self.shell)

    def quit(self):
        self.running.send_signal(signal.SIGINT)
        try:
            self.running.wait(1)
        except subprocess.TimeoutExpired:
            command = self.command if isinstance(self.command, str) else " ".join(self.command)
            print(f"'{command}' still running, terminating")
            self.running.terminate()
            try:
                self.running.wait(1)
            except subprocess.TimeoutExpired:
                print(f"'{command}' ignoring SIGTERM, killing")
                self.running.kill()
                self.running.wait()

        return self.running.poll()


class ZmqPull:

    def __init__(self, endpoint = "tcp://*:5555"):
        self.context = zmq.Context.instance()
        self.endpoint = endpoint

    def pull_msg(self):
        self.socket = self.context.socket(zmq.PULL)
        # https://stackoverflow.com/questions/7538988/zeromq-how-to-prevent-infinite-wait
        self.socket.RCVTIMEO = 200  # milliseconds

        self.socket.LINGER = 100
        self.socket.bind(self.endpoint)
        with contextlib.closing(self.socket):
            try:
                channel, raw = self.socket.recv_multipart()
                message = osgar.lib.serialize.deserialize(raw)
                # print(channel, message)
                return message

            except zmq.error.Again:
                    pass


def main():
    incomme = ZmqPull()
    launcher = OsgarLaunch()
    running = False
    while True:
        message = incomme.pull_msg()
        if message:
            if message == "quit":
                assert running == True, running
                launcher.quit()
                running = False
            else:
                assert running == False
                launcher.start(message)
                running = True

        time.sleep(2)


if __name__ == "__main__":
    main()
