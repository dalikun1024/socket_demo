import socket
import struct
import _thread


udp_server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
port = 12345
udp_server_socket.bind(('0.0.0.0', port))
reserved_data = b''
while True:
    client_bytes_address_pair = udp_server_socket.recvfrom(1024)
    if len(client_bytes_address_pair) == 0:
        continue
    client_data = reserved_data + client_bytes_address_pair[0]
    reserved_data = b''
    for msg_data in client_data.split(b'\n\n\t\t'):
        if (len(msg_data) == 0):
            continue
        fmt = 'i%ds' % (len(msg_data)-4)
        message_data = struct.unpack(fmt, msg_data)
        print("message_data: type: ", message_data[0])
        print("message_data: len: ", len(message_data[1]))
        if message_data[0] == 0:
            if (len(message_data[1]) != 16):
                reserved_data = msg_data
                continue
            gyro_data = struct.unpack('4f', message_data[1])
            print('gyro_data: ', gyro_data)
        elif message_data[0] == 1:
            if (len(message_data[1]) != 20):
                reserved_data = msg_data
                continue
            gsensor_data = struct.unpack('5f', message_data[1])
            print('gsensor_data: ', gsensor_data)
        elif message_data[0] == 2:
            if (len(message_data[1]) != 112):
                reserved_data = msg_data
                continue
            gps_data = struct.unpack('=2i3f2d32s2f3i6f', message_data[1])
            print('gps_data: ', gps_data)
        else:
            print("invalid socket data type")
s.close()