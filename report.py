import socket
import os
import os.path
import sys

host = sys.argv[1]
port = 26505
msg_size = 1024
num = 0
report_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
report_socket.connect((host,port))

def report():
	if sys.argv[2] == '-k':
		report_socket.send('DIEDIEDIE#')
	else:
		report_socket.send('RPTNUM#')
		data_in = report_socket.recv(msg_size)
		msg = data_in.split('#')
		num = int(msg[0])
		report_socket.send('RPT#')
		data_in = report_socket.recv(msg_size)
		a_list = eval(data_in)
		p_list = a_list[1:num]
		print repr(p_list)
if __name__ == '__main__':
	report()

