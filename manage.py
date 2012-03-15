import socket
import os
import os.path
import subprocess
import select


def calc_range(start, t_ops):
	MAX_VALUE = 4000000000
	while (t_ops > 0):
		t_ops -= start
		start += 1
	if (start > MAX_VALUE):
		start = MAX_VALUE
	return start


def manage():
	print 'server started\n'
	MAX_VALUE = 4000000000
	HOST = ''
	PORT = 26505
        backlog = 5
	msg_size = 1024
	r_start = 0
	r_end = 0
	new_perfects = 0
	total_perfects = 0
	p_to_add = 0
	perfects_list = []
	list_crawl = 0

	serv_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	serv_sock.bind((HOST,PORT))
	serv_sock.listen(backlog)
	sockets = [serv_sock]

	while True:
		sock_in, sock_out, sock_err = select.select(sockets , [] , [] )
		for client in sock_in:
			if client is serv_sock:
				client, client_addr = serv_sock.accept()
				print 'got connection from ', client_addr
				sockets.append(client)
			else:
				data_in = client.recv(msg_size)
				if data_in:
					msg = data_in.split('#')
					
					if msg[0] == 'REQWSPEC':
						c_ops = int(msg[1])
						op_time_int = int (msg[2])
						op_time = float(op_time_int/100)
						t_ops = c_ops * (5/op_time)
						r_start = r_end + 1
						r_end = calc_range(r_start,t_ops)
						client.send('FPN#'+str(r_start)+'#'+str(r_end))
						print'Client sent '+str(r_start)+' to '+str(r_end)
					elif msg[0] == 'RTNP':
						new_perfects = int(msg[1])
						for i in range(2,2+new_perfects):
							p_to_add = int(msg[i])
							perfects_list[list_crawl] = p_to_add
							list_crawl += 1
						print 'Recieved '+str(new_perfects)+' numers'

				else:
					client.close()
					sockets.remove(client)

if __name__ == '__main__':
	manage()
