=======================================================================
                            README-vnc
                          Linux for Tegra
               Configuring VNC from the command-line
=======================================================================

A VNC server allows access to the graphical display of a Linux for Tegra system
over the network. This allows you to work physically remote from the Linux for
Tegra system, and avoids the need to connect an HDMI display, USB keyboard, or
mouse.

All commands specified below should be executed from a terminal on the Linux
for Tegra system. This could be a serial port, an SSH session, or a graphical
terminal application running on the HDMI display.

----------------------------------------------------------------------
Installing the VNC Server
----------------------------------------------------------------------

It is expected that the VNC server software is pre-installed. Execute the
following commands to ensure that it is:

sudo apt update
sudo apt install vino

----------------------------------------------------------------------
Enabling the VNC Server
----------------------------------------------------------------------

Execute the following commands to enable the VNC server:

# Inform the system which graphic display to interact with
export DISPLAY=:0
# Enable the VNC server
gsettings set org.gnome.Vino enabled true
gsettings set org.gnome.Vino prompt-enabled false
gsettings set org.gnome.Vino require-encryption false
# Reboot the system so that the settings take effect
sudo reboot

----------------------------------------------------------------------
Connecting to the VNC server
----------------------------------------------------------------------

Use any standard VNC client application to connect to the VNC server that is
running on Linux for Tegra. Popular examples for Linux are gvncviewer and
remmina. Use your own favorite client for Windows or MacOS.

To connect, you will need to know the IP address of the Linux for Tegra system.
Execute the following command to determine the IP address:

ifconfig

Search the output for the text "inet addr:" followed by a sequence of four
numbers, for the relevant network interface (e.g. eth0 for wired Ethernet,
wlan0 for WiFi, or l4tbr0 for the USB device mode Ethernet connection).

Once you are connected to the VNC server, execute the following command to
display the VNC server's graphical configuration tool, and then configure a
password for the VNC connection:

vino-preferences
