"""
    Usage:
        python run_jetson.py <config> <ip>
"""
import contextlib
import zmq
import time
import osgar.lib.serialize
import sys
from threading import Thread


def push_msg(msg, endpoint):
    context = zmq.Context.instance()
    socket = context.socket(zmq.PUSH)

    # https://stackoverflow.com/questions/7538988/zeromq-how-to-prevent-infinite-wait
    socket.SNDTIMEO = int(1000)  # convert to milliseconds
    socket.LINGER = 100  # milliseconds
    socket.connect(endpoint)

    try:
        with contextlib.closing(socket):
            raw = osgar.lib.serialize.serialize(msg)
            socket.send_multipart([bytes("control_msg", 'ascii'), raw])
    except zmq.ZMQError as e:
        print(e)

class RunJetson:
    def __init__(self, config, bus):
        self.input_thread = Thread(target=self.run_input, daemon=True)
        self.bus = bus
        bus.register('msg')

        host = config.get('host', '127.0.0.1')
        self.task = config.get('task')  # path to config file
        assert self.task is not None
        self.endpoint = f"tcp://{host}:5550"


    def run_input(self):
        push_msg(self.task, self.endpoint)
        while self.bus.is_alive():
            self.bus.sleep(2)

    def start(self):
        self.input_thread.start()

    def join(self, timeout=None):
        push_msg('quit', self.endpoint)
        self.input_thread.join(timeout=timeout)

    def request_stop(self):
        self.bus.shutdown()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit()

    host = sys.argv[2]
    endpoint = f"tcp://{host}:5550"
    push_msg(sys.argv[1], endpoint)  # config file and ip
    time.sleep(10)
    push_msg("quit", endpoint)
