=======================================================================
                            README-wifi
                          Linux for Tegra
               Configuring WiFi from the command-line
=======================================================================

A Linux for Tegra device may be connected to a WiFi network using different
tools, depending on whether you interact with Linux for Tegra using the
command-line (i.e. serial port, or SSH connection), or a graphical interface
via an HDMI display.

All commands specified below should be executed from a terminal on the Linux
for Tegra system. This could be a serial port, an SSH session, or a graphical
terminal application running on the HDMI display.

----------------------------------------------------------------------
Installing Network Manager
----------------------------------------------------------------------

It is expected that the Network manager software is pre-installed. Execute the
following command to ensure that it is:

sudo apt update
sudo apt install network-manager
sudo service NetworkManager start

----------------------------------------------------------------------
Using the Command Line
----------------------------------------------------------------------

Execute the following command, having replaced SSID with the name of the
network you wish to connect to, and PASSWORD with the password/passphrase for
that network.

sudo nmcli device wifi connect 'SSID' password 'PASSWORD'

----------------------------------------------------------------------
Using the Graphical Interface
----------------------------------------------------------------------

A network configuration icon will appear at the top-right corner of the
display. Click this with the mouse to see a list of WiFi networks, then click
the network name you wish to connect to, and follow the on-screen prompts.
