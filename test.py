import socket
import time
import re
client=socket.socket()
client.connect(("127.0.0.1", 1234))

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

msg="SYST\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="TYPE I\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="PASV\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)


msg="PASV\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)
datas=re.split(r"[,()]", data)
print(datas)
port=int(datas[5])*256+int(datas[6])
print(port)

datafd=socket.socket()
msg="STOR lalala22.txt\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)
datafd.connect(("127.0.0.1", port))
datafd.send("lalallllla")
datafd.close()
data=client.recv(1002400)
print(data)

msg="PORT 127,0,0,1,255,254\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

datafd=socket.socket()
datafd.bind(("127.0.0.1", 65534))
datafd.listen(1)
msg="RETR lalala.txt\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)
data_conn, addr = datafd.accept()
data=data_conn.recv(1002400)
print(data)
data_conn.close()
datafd.close()
data=client.recv(1002400)
print(data)

msg="PASV\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)
datas=re.split(r"[,()]", data)
print(datas)
port=int(datas[5])*256+int(datas[6])
print(port)

datafd=socket.socket()
msg="LIST test\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)
datafd.connect(("127.0.0.1", port))
data=datafd.recv(1002400)
print(data)
data=datafd.recv(1002400)
print(data)
datafd.close()
data=client.recv(1002400)
print(data)

msg="MKD test2\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="CWD ..\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="CWD ./test\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="CWD .f/test\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="CWD ..\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="CWD test\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="CWD /test2\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

msg="QUIT\r\n"
client.send(msg)
data=client.recv(1002400)
print(data)

client.close()