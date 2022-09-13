"""
    Usage:
        python run_jetson.py <config> <ip>
"""
import contextlib
import zmq
import time
import osgar.lib.serialize
import sys


def push_msg(data, ip = "127.0.0.1"):
    endpoint = f"tcp://{ip}:5550"
    context = zmq.Context.instance()
    socket = context.socket(zmq.PUSH)

    # https://stackoverflow.com/questions/7538988/zeromq-how-to-prevent-infinite-wait
    socket.SNDTIMEO = int(1000)  # convert to milliseconds
    socket.LINGER = 100  # milliseconds
    socket.connect(endpoint)

    try:
        with contextlib.closing(socket):
            raw = osgar.lib.serialize.serialize(data)
            socket.send_multipart([bytes("control_msg", 'ascii'), raw])
    except zmq.ZMQError as e:
        print(e)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit()
    if len(sys.argv) == 3:
        push_msg(sys.argv[1], sys.argv[2])
    else:
        push_msg(sys.argv[1])
    time.sleep(10)
    push_msg("quit")