import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.bind(("",12345))

while True:
    (data, addr) = sock.recvfrom(4096)
    print("{} says: {}".format(addr, data))
    sock.sendto(b"kiaone", addr)
