#!/bin/bash

slattach -L -s 9600 -p slip /dev/ttyUSB0 &
sleep 1
ifconfig sl0 172.26.79.215 dstaddr 172.26.79.214 mtu 1500 
sleep 1
tcpdump -i sl0 -X
