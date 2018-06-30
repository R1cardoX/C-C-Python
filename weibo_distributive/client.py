import json
from socket import *
BUFSIZE = 1024
def get_data_from_server(tcpCliSock):
    json_dict = ""
    while True:
        while True:
            data = tcpCliSock.recv(BUFSIZE).decode()
            json_dict = json_dict + data
            if not data:
                break
            if data[-3:] == 'END':
                break
        if len(json_dict) > 0:
            break
    datas = json.loads(json_dict[0:-3])
    print(datas)
    return datas
    
def post_data_to_server(tcpCliSock,datas):
    json_dict = json.dumps(datas) + 'END'
    for i in range(0,len(json_dict),BUFSIZE):
        data = json_dict[i:i+BUFSIZE]
        tcpCliSock.send(data.encode())  
