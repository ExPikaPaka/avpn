#!/bin/bash

# Configuration file
source config

# Check if config file is correct
if [ -z "$TUN_INTERFACE" ] || [ -z "$TUN_IP" ] || [ -z "$TUN_NETMASK" ] || [ -z "$MAIN_INTERFACE" ]; then
    echo "Error: config file is corrupted!"
    exit 1
fi

# Removing IP tables
iptables -t nat -D POSTROUTING -s $TUN_IP/$TUN_NETMASK -o $MAIN_INTERFACE -j MASQUERADE
if [ $? -ne 0 ]; then
    echo "Error: failed to remove IP tables."
    exit 1
fi

# Enabling IP forwarding
echo 0 > /proc/sys/net/ipv4/ip_forward
if [ $? -ne 0 ]; then
    echo "Error: failed to disable IP forwarding."
    exit 1
fi

# Removing IP address and mask for TUN device
ip addr del $TUN_IP/$TUN_NETMASK dev $TUN_INTERFACE
if [ $? -ne 0 ]; then
    echo "Error: failed to remove IP and mask from the TUN device."
    exit 1
fi

# Turning down TUN device
ip link set $TUN_INTERFACE down
if [ $? -ne 0 ]; then
    echo "Error: failed to turn down TUN device."
    exit 1
fi

# Creating TUN device
ip tuntap del mode tun $TUN_INTERFACE
if [ $? -ne 0 ]; then
    echo "Error: failed to create TUN device."
    exit 1
fi

echo "TUN interface was removed successfully."
echo "TUN_INTERFACE=$TUN_INTERFACE"
echo "TUN_IP=$TUN_IP"
echo "TUN_NETMASK=$TUN_NETMASK"
echo "MAIN_INTERFACE=$MAIN_INTERFACE"
exit 0

