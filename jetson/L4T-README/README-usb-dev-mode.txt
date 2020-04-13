=======================================================================
                        README-usb-dev-mode
                          Linux for Tegra
                          USB Device Mode
=======================================================================

The USB device mode configuration script configures the Tegra USB flashing port
to act as a USB device and implement various protocols. The script allows a USB
host system to interact with the Tegra device using a USB cable. The following
implemented protocols can be used at the same time:

- Ethernet: allows system login using SSH and high-bandwidth file copying using
  SFTP.
- UART/Serial: allows system login using a terminal application.
- USB Mass Storage: Similar in concept to a USB memory stick.

----------------------------------------------------------------------
Using Ethernet Protocol
----------------------------------------------------------------------
Linux for Tegra implements two types of Ethernet devices to support various
operating systems running on the USB host machine. It is safe to ignore system
errors for missing drivers for an RNDIS or an ECM device. If your system
supports both devices, either device can be used. However, do not configure
both devices at the same time.

Linux for Tegra assigns a link-local IPv6 address of fe80::1 to the USB
Ethernet device(s). You should not have to configure your host machine in order
to use an IPv6 link-local address. If you connect multiple Jetson devices to
the same host machine, each Jetson will use the same IPv6 link-local address.
However, you may still connect to any connected Jetson, since use of IPv6
link-local addresses also requires specifying which network interface to
connect over, and there is a unique interface for each Jetson.

Linux for Tegra assigns a static IPv4 address of 192.168.55.1 to Jetson, and
runs a DHCP server to automatically assign an IPv4 address to your host
machine. This provides point-to-point connectivity.

Linux for Tegra configures a very low priority default IPv4 route on the USB
Ethernet device(s), and configures Google's public DNS server (8.8.8.8) as the
fallback DNS server for use when no other network connection is available.
This allows your host machine to act as an Intranet or Internet gateway for
Jetson.

If you connect multiple Jetson devices to the same host machine, each Jetson
will use the same IPv4 address. This prevents communication with all but one
Jetson machine, since your host operating system will determine which Jetson
it communicates with. To solve this, edit the Jetson-based script that sets up
the network, and assign a unique network address to each Jetson.

Once the Ethernet device is configured, use SSH to connect to the Linux for
Tegra device.

SSH is natively available on Linux and Mac operating systems.
To connect using IPv6, execute the command:
ssh nvidia@fe80::1%usb0
To connect using IPv4, execute the command:
ssh nvidia@192.168.55.1

On Windows, use the PuTTY application.

Ethernet on Linux
----------------------------------------------------------------------
Two USB Ethernet devices are created and the required drivers are automatically
activated. These devices are typically named usb0 and usb1, depending on how
your Linux distribution is configured, and what other hardware is attached to
the system.

To view available Ethernet devices, execute the command:
/sbin/ifconfig -a

Knowledge of the Ethernet device name is required in order to use IPv6
link-local addresses.

Use the following procedure to configure your host as a gateway for Jetson.

1. Enable IP forwarding with your host's configuration tools, or by running
   the following command as root:

   echo 1 > /proc/sys/net/ipv4/ip_forward

   If your host system is connected to multiple networks at the same time,
   please consider any security implications of this configuration change.

   Depending on your Linux distribution, you may be able to make this change
   permanent by editing /etc/sysctl.conf.

2. Enable Network Address Translation (NAT) using your host's configuration
   tools to do this, or by running the following command as root:

   iptables -t nat -A POSTROUTING -o eth0 -j SNAT --to 192.168.1.100

   where:
    eth0 is the name of your host's upstream Ethernet interface.
    192.168.1.100 is your host's IP address on that interface.

Depending on your Linux distribution, you may have to run this command every
time your host is rebooted.

Ethernet on Windows
----------------------------------------------------------------------
An RNDIS USB Ethernet device is created, and the required driver is
automatically installed. You can safely ignore the message that the ECM device
is unknown.

Ethernet on Mac
----------------------------------------------------------------------
An ECM USB Ethernet device is created, and the required driver is automatically
activated. Since the Mac operating system does not support the RNDIS device,
the device is not activated by default.

Changing the IPv4 Address
----------------------------------------------------------------------
Edit /opt/nvidia/l4t-usb-device-mode/nv-l4t-usb-device-mode.sh on Jetson to
change the IPv4 network parameters. The following variables must be changed,
and must maintain consistent values:
- net_ip
- net_mask
- net_net
- net_dhcp_start
- net_dhcp_end

----------------------------------------------------------------------
Using the Serial Port
----------------------------------------------------------------------
Linux for Tegra implements a single USB serial port, and presents a login
prompt on the port. You may log into the system and run shell commands over
this port.

Any standard serial terminal application may be used with the port. For
example:
- Screen (Linux/Mac)
- Picocom (Linux)
- Tera Term (Windows)
- HyperTerminal (Windows)

Since this is an emulated serial port, any baud rate will work. The port must
be configured for 8N1 (8-bits per character, no stop bit, 1 parity bit).

Serial Port on Linux
----------------------------------------------------------------------
Linux creates a device such as /dev/ttyACM0 for the serial port. To locate the
exact name:
1. Launch a shell prompt.
2. Run the command: ls /dev.
3. Search for "ACM".

Serial Port on Windows
----------------------------------------------------------------------
Windows creates a standard COM port device for the serial port.

On systems using an operating system prior to Windows 10, you must manually
install the driver for the serial port. To install the driver:
1. Open Device Manager.
2. Locate the "CDC Serial" device.
3. Right-click the device and select "update Driver Software".
4. Select to Browse for the driver software.
5. Set the path to the USB disk drive that is implemented by your Linux for
   Tegra device.

Windows will locate the required .inf file and install the relevant drivers for
the serial port.

Serial Port on Mac
----------------------------------------------------------------------
Mac OS creates a device such as /dev/tty.usbmodem1232 for the serial port. To
locate the exact name:
1. Launch a shell prompt.
2. Run the command: ls /dev.
3. Search for "tty.usbmodem".

----------------------------------------------------------------------
Using USB Mass Storage
----------------------------------------------------------------------
Linux for Tegra implements a USB Mass Storage device, known as a USB disk or
USB memory stick, that is natively supported by all operating systems. The USB
disk automatically appears in the graphical filesystem browser such as:
- Linux desktop
- Windows Explorer
- Mac Finder

On Linux or Mac, you can manually mount the disk from the command line, just
like a USB storage device.

The USB storage device is read-only and contains various driver files and Linux
for Tegra documentation.

----------------------------------------------------------------------
Disabling USB Device Mode Temporarily
----------------------------------------------------------------------

To disable USB device mode temporarily:
1. On the Linux for Tegra system, launch a shell prompt.
2. Log in as root.
3. Execute the command:
   service nv-l4t-usb-device-mode stop

To re-enable USB device mode:
1. On the Linux for Tegra system, launch a shell prompt.
2. Log in as root.
3. Execute the command:
   service nv-l4t-usb-device-mode start

USB device mode will be re-enabled automatically the next time the system is
rebooted.

----------------------------------------------------------------------
Disabling USB Device Mode Permanently
----------------------------------------------------------------------
To disable USB device mode permanently:
1. On the Linux for Tegra system, launch a shell prompt.
2. Log in as root.
3. Execute the commands:
   service nv-l4t-usb-device-mode stop
   systemctl disable nv-l4t-usb-device-mode.service

This is equivalent to deleting the following files:

/etc/systemd/system/nv-l4t-usb-device-mode.service
/etc/systemd/system/multi-user.target.wants/nv-l4t-usb-device-mode.service

To re-enable USB device mode, execute the commands:

systemctl enable /opt/nvidia/l4t-usb-device-mode/nv-l4t-usb-device-mode.service
service nv-l4t-usb-device-mode start
