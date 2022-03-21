#!/usr/bin/env python3

import socket
import sys
import time

HOST, PORT = "localhost", 50007

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))
while 1:
  #print ("Send: %s" % x)
  #s.send(str.encode(str(x)))
  message = s.recv(1000)
  print (message.decode('ascii'))
#time.sleep(1)
