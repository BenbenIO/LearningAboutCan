#!/bin/bash
echo "Starting vcan0"

sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

echo ifconfig | grep "vcan0"

echo "please run 'candump vcan0' to monitor vcan0 messages" 
