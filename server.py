import socket
import struct

s = socket.socket()
port = 12345

s.bind(('127.0.0.1', port))
s.listen(1)
c, addr = s.accept()
print("connection address: ", addr)

reserved_data = b''
while True:
    client_data = c.recv(1024)
    if len(client_data) == 0:
        continue
    client_data = reserved_data + client_data
    for msg_data in client_data.split(b'\n'):
        if (len(msg_data) == 0):
            continue
        fmt = 'i%ds' % (len(msg_data)-4)
        message_data = struct.unpack(fmt, msg_data)
        if message_data[0] == 0:
            if (len(message_data[1]) != 16):
                reserved_data = message_data[1]
                continue
            gyro_data = struct.unpack('4f', message_data[1])
            print('gyro_data: ', gyro_data)
        elif message_data[0] == 1:
            if (len(message_data[1]) != 20):
                reserved_data = message_data[1]
                continue
            gsensor_data = struct.unpack('5f', message_data[1])
            print('gsensor_data: ', gsensor_data)
        elif message_data[0] == 2:
            if (len(message_data[1]) != 84):
                reserved_data = message_data[1]
                continue
            print(len(message_data[1]))
            gps_data = struct.unpack('=2i3f2d32s2f2i', message_data[1])
            print('gps_data: ', gps_data)
        else:
            print("invalid socket data type")
    
    # c.send(b'server send data')
    # c.close()