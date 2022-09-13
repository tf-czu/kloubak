"""
    Osgar launcher
"""

import subprocess
import signal
import contextlib
import zmq
import time
import osgar.lib.serialize
from threading import Thread

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
    def __init__(self, endpoint = "tcp://*:5550"):
        self.endpoint = endpoint
        self.message = None

    def pull_msg(self):
        context = zmq.Context.instance()
        socket = context.socket(zmq.PULL)
        # https://stackoverflow.com/questions/7538988/zeromq-how-to-prevent-infinite-wait
        socket.RCVTIMEO = 200  # milliseconds

        socket.LINGER = 100
        socket.bind(self.endpoint)
        with contextlib.closing(socket):
            while True:
                try:
                    channel, raw = socket.recv_multipart()
                    message = osgar.lib.serialize.deserialize(raw)
                    if message:
                        self.message = message

                except zmq.error.Again:
                        pass
                time.sleep(2)


def main():
    incomme = ZmqPull()
    Thread(target=incomme.pull_msg, daemon=True).start()
    launcher = OsgarLaunch()
    running = False
    while True:
        message = incomme.message
        if message:
            if message == "quit" and running:
                launcher.quit()
                running = False
            elif not running and message != "quit":
                launcher.start(message)
                running = True

        time.sleep(2)


if __name__ == "__main__":
    main()
