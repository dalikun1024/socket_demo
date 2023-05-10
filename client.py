import socket
s = socket.socket()
host = socket.gethostname()
port = 12345
s.connect(('192.168.158.100', port))
s.send(b'this is client dataxxxx')
print(s.recv(1024))
s.close()