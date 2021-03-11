# -*- coding: utf-8 -*-

import socket

sock = socket.socket()
sock.bind(('', 9090))
sock.listen(1)
conn, addr = sock.accept()

print ('connected:', addr)

while 1:
    data = conn.recv(1024)
    udata = data.decode("utf-8")
    print ("Data: " + udata)
    
conn.close()