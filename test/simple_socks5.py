import socket
import struct
import select

def socks5_handshake(client_socket):
    # SOCKS5 handshake
    handshake_data = client_socket.recv(256)
    if len(handshake_data) < 3 or handshake_data[0] != 0x05:
        raise Exception("Invalid SOCKS5 handshake")

    # Respond with no authentication required
    client_socket.sendall(b'\x05\x00')

def socks5_request(client_socket):
    # SOCKS5 request
    request_data = client_socket.recv(256)
    if len(request_data) < 10 or request_data[0] != 0x05:
        raise Exception("Invalid SOCKS5 request")

    cmd = request_data[1]
    if cmd != 0x01:  # Only support CONNECT command
        client_socket.sendall(b'\x05\x07\x00\x01')  # Command not supported
        raise Exception("Unsupported command")

    addr_type = request_data[3]
    if addr_type == 0x01:  # IPv4
        dest_addr = socket.inet_ntoa(request_data[4:8])
        dest_port = struct.unpack('>H', request_data[8:10])[0]
    elif addr_type == 0x03:  # Domain name
        addr_len = request_data[4]
        dest_addr = request_data[5:5 + addr_len].decode('utf-8')
        dest_port = struct.unpack('>H', request_data[5 + addr_len:7 + addr_len])[0]
    else:
        client_socket.sendall(b'\x05\x08\x00\x01')  # Address type not supported
        raise Exception("Unsupported address type")

    return dest_addr, dest_port

def socks5_connect(client_socket, dest_addr, dest_port):
    try:
        dest_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        dest_socket.connect((dest_addr, dest_port))
    except Exception as e:
        client_socket.sendall(b'\x05\x03\x00\x01')  # Connection refused
        raise e

    # Send success response
    client_socket.sendall(b'\x05\x00\x00\x01' + socket.inet_aton('0.0.0.0') + struct.pack('>H', 0))

    return dest_socket

def socks5_proxy(client_socket, dest_socket):
    try:
        while True:
            r, w, e = select.select([client_socket, dest_socket], [], [])
            if client_socket in r:
                data = client_socket.recv(4096)
                if not data:
                    break
                dest_socket.sendall(data)
            if dest_socket in r:
                data = dest_socket.recv(4096)
                if not data:
                    break
                client_socket.sendall(data)
    finally:
        client_socket.close()
        dest_socket.close()

def socks5_server(host='172.30.101.199', port=8100):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(5)
    print(f"SOCKS5 server started on {host}:{port}")

    try:
        while True:
            client_socket, addr = server_socket.accept()
            print(f"Connection established from {addr}")
            try:
                socks5_handshake(client_socket)
                dest_addr, dest_port = socks5_request(client_socket)
                dest_socket = socks5_connect(client_socket, dest_addr, dest_port)
                socks5_proxy(client_socket, dest_socket)
            except Exception as e:
                print(f"Error: {e}")
                client_socket.close()
    except KeyboardInterrupt:
        print("Server shutting down")
    finally:
        server_socket.close()

if __name__ == "__main__":
    socks5_server()
