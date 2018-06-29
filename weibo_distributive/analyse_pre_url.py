import time
from bs4 import BeautifulSoup
import re
import json
import pandas as pd
import numpy as np
import multiprocessing as mp

USER_URL  = 100
PRE_URL = 101
USER_HTML = 102
PRE_HTML = 103
HOST = '127.0.0.1'
PORT = 8000
BUFSIZE = 1024
ADDR = (HOST,PORT)

def analyse_some_att(html):
    if html is None:
        return ""
    html1 = re.sub(re.compile(r'\\/'),"/",html)
    html2 = re.sub(re.compile(r'\\"'),"\"",html1)
    html3 = re.sub(re.compile(r'\\t'),"\t",html2)
    html4 = re.sub(re.compile(r'\\n'),"\n",html3)
    html5 = re.sub(re.compile(r'\\r'),"\r",html4)
    p = re.compile(r'action-data=\\"uid=(\d{1,15})&nick=(.+?)\\">')
    p2 = re.compile(r'<title>([^<>]+)</title>')
    try:
        title = p2.search(html5).group(1).split("的微博")[0]
    except:
        title = ""
    try:
        att = p.findall(html)
        url_list = []
        for _id in att:
            url = 'https://weibo.com/u/' + _id[0]# + '?from=myfollow_all'
            url_list.append(url)
            print("Get the user url:",url,"\t\tfrom ",title)
    except:
        return ""
    return set(url_list)


def main():
    pool = mp.Pool(4)
    tcpCliSock = socket(AF_INET,SOCK_STREAM)
    tcpCliSock.connect(ADDR)
    tcpCliSock.send(str(PRE_HTML).encode())
    while True:
        buf = tcpCliSock.recv(100)
        if 'OK' in buf:
            break
    while True:
        htmls = get_data_from_server(tcpCliSock)
        while len(unseen) != 0 or len(att_urls) <= 10000 or user.count <= 10000:
            parse_jobss = [pool.apply_async(analyse_some_att,args=(html,)) for html in htmls]
            url_lists = [j.get() for j in parse_jobss]
        post_data_to_server(tcpCliSock,url_lists)

def get_data_from_server(tcpCliSock):
    json_dict = ""
    while True:
        data = tcpCliSock.recv(BUFSIZE).decode()
        if not data:
            break
        json_dict = json_dict + datas
    datas = json.loads(json_dict)
    return datas
    
def post_data_to_server(tcpCliSock,datas):
    json_dict = json.dumps(datas)
    for i in range(0,len(json_dict),BUFSIZE):
        data = json_dict[i:BUFSIZE]
        tcpCliSock.send(data.encode())

if __name__ == "__main__":
   main() 
    
