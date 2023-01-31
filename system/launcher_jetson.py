"""
    Osgar launcher
    The launcher_jetson is running on background and it receives messages through ZMQ (strings).
    There are two types of messages: "quit" (terminate program) and config file which is passed to osgar.record.
"""

import subprocess
import signal
import contextlib
import zmq
import time
import osgar.lib.serialize
from threading import Thread

class OsgarLauncher:
    def __init__(self):
        self.command = ["python3", "-m", "osgar.record"]
        self.shell = False  # or True?? is it needed?
        self.running = None

    def start(self, config_file):
        if self.running is not None:
            assert self.running.poll() is not None
            self.running = None
        self.command.append(config_file)
        self.running = subprocess.Popen(self.command, shell=self.shell, stdout=subprocess.PIPE)

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
                self.message = None
                try:
                    channel, raw = socket.recv_multipart()
                    message = osgar.lib.serialize.deserialize(raw)
                    if message:
                        self.message = message

                except zmq.error.Again:
                        pass
                time.sleep(2)


def main():
    income = ZmqPull()
    Thread(target=income.pull_msg, daemon=True).start()
    launcher = OsgarLauncher()
    is_running = False
    last_tick = None
    while True:
        message = income.message
        # print("income message", message)
        if last_tick and not message:
            if time.time() - last_tick > 5:
                if not is_running:
                    last_tick = None
                    continue
                message = "quit"
                print("delay, terminate the process")
        if message:
            if message == "tick":
                last_tick = time.time()

            if message == "quit" and is_running:
                launcher.quit()
                is_running = False
                last_tick = None

            elif not is_running and message not in ["quit", "tick"]:
                launcher.start(message)
                is_running = True

        time.sleep(1.5)


if __name__ == "__main__":
    main()
