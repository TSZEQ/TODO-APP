import socket

# addressing information of target
IPADDR = 'localhost'
PORTNUM = 8888

# enter the data content of the UDP packet as hex
PACKETDATA = "Hello World! A udp packet."
#PACKETDATA = 'a' *  1024

# initialize a socket, think of it as a cable
# SOCK_DGRAM specifies that this is UDP
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)

# connect the 