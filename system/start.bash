#!/bin/bash
# Copy it to /root/ (APU)
# run it: source start.bash
systemctl stop serial-getty@ttyS0.service
dmesg -D
ifup br0
echo "Set date on Jetson nano units and run launchers"
sleep 1
ssh k2jetson_front -t "sudo date +%c -s '$(date +%c)'"
ssh k2jetson_front -t "screen -dmL python3 git/kloubak/system/launcher_jetson.py && sleep 1"
ssh k2jetson_rear -t "sudo date +%c -s '$(date +%c)'"
ssh k2jetson_rear -t "screen -dmL python3 git/kloubak/system/launcher_jetson.py && sleep 1"
cd /home/robot/git/osgar
screen
