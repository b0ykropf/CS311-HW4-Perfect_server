import socket
import os
import os.path
import subprocess
import shlex

def store():
	MAX_VALUE = 4000000000
	num_list = range(1,MAX_VALUE)
	

def manage():
	print 'server started\n'
	HOST = ''
	PORT = 26505
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.bind((HOST,PORT))

	s.listen(5)
	while True:
		c, addr = s.accept()
		print 'got connection from ', addr
		c.send( 'thanks for connecting\n')
		c.close()

if __name__ == '__main__':
	manage()
