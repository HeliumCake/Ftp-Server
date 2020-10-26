import socket
import time
client=socket.socket()
client.connect(("127.0.0.1", 6789))

data=client.recv(1002400)
print(data)

msg="USER anonymous\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="PASS 123456@123.com\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="RNTO\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

client.close()