#!/bin/bash

# Configuration file
source config

# Check if config file is correct
if [ -z "$TUN_INTERFACE" ] || [ -z "$TUN_IP" ] || [ -z "$TUN_NETMASK" ] || [ -z "$MAIN_INTERFACE" ]; then
    echo "Error: config file is corrupted!"
    exit 1
fi

# Creating TUN device
ip tuntap add mode tun $TUN_INTERFACE
if [ $? -ne 0 ]; then
    echo "Error: failed to create TUN device."
    exit 1
fi

# Enabling TUN device
ip link set $TUN_INTERFACE up
if [ $? -ne 0 ]; then
    echo "Error: failed to turn up TUN device."
    exit 1
fi

# Configuring IP address and mask for TUN device
ip addr add $TUN_IP/$TUN_NETMASK dev $TUN_INTERFACE
if [ $? -ne 0 ]; then
    echo "Error: failed to set IP and mask to the TUN device."
    exit 1
fi

# Enabling IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward
if [ $? -ne 0 ]; then
    echo "Error: failed to enable IP forwarding."
    exit 1
fi

# Configuring IP tables
iptables -t nat -A POSTROUTING -s $TUN_IP/$TUN_NETMASK -o $MAIN_INTERFACE -j MASQUERADE
if [ $? -ne 0 ]; then
    echo "Error: failed to configure IP tables."
    exit 1
fi

# Success message
echo "TUN interface was configured successfully."
echo "TUN_INTERFACE=$TUN_INTERFACE"
echo "TUN_IP=$TUN_IP"
echo "TUN_NETMASK=$TUN_NETMASK"
echo "MAIN_INTERFACE=$MAIN_INTERFACE"
exit 0

