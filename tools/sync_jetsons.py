import subprocess


def main():

    all_jetsons = {
        "k2_front": "k2jetson_front:/home/jetson/git/logs/",
        "k2_rear": "k2jetson_rear:/home/jetson/git/logs/",
    }

    import argparse
    parser = argparse.ArgumentParser(description='rsync logs from jetsons to apu')
    parser.add_argument('jetson', nargs='*', choices=list(all_jetsons.keys()) + [[]], default=[])
    args = parser.parse_args()

    if args.jetson == []:
        jetsons = all_jetsons
    else:
        jetsons = {k: all_jetsons[k] for k in args.robots}

    home = "/home/robot/git/logs"
    for jetson, logs in jetsons.items():
        print(f"Syncing {jetson}:")
        a = subprocess.call(["rsync", "-v", "--append", "--progress", "--recursive", f"{logs}", f"{home}/{jetson}"])
        if a == 0:
            print("OK")
        else:
            print("Failed")
        print()

if __name__ == "__main__":
    main()
