# -*- coding: UTF-8 -*-
import requests
import json
import five_e
import zhihu
import it
import os
import time
import random

class Snd:
    APPID = "wx8368c8bd768f22e1"
    appsecret = "2e43382589842298b984c6b871851c8d"
    p_recv = "./fifo_recv"
    p_send = "./fifo_send"
    def __init__(self):
        self.access_token_url = "https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=" +self.APPID + "&secret=" + self.appsecret
        self.access_token = self.get_access_token()
        self.post_url = "https://api.weixin.qq.com/cgi-bin/message/mass/send?access_token=" + self.access_token
        self.user_url = "https://api.weixin.qq.com/cgi-bin/user/get?access_token=" + self.access_token + "&next_openid="
    def get_data(self):
        datas = []
        datas.append(five_e.Match.get_data())
        datas.append(it.Match.get_data())
        datas.append(it.Match.get_data())
        datas.append(zhihu.Match.get_key())
        datas.append(zhihu.Match.get_key())
        for data in datas:
            print(data)
        return datas
    def get_main_text(self,data):
        datas = []
        #todo
        return datas
    def shield_sensitive_text(self,data):
        if os.access(p_send, os.F_OK) == False:
            os.mkfifo(p_send) 
        fp_r = os.open(p_recv,os.O_RDWR)
        fd_w = os.open(p_send,os.O_RDWR)
        r_data = ""
        for i in range(0,len(data),1024)
            os.write(fp_r,data[i,i+1024].encode("utf-8"))
            r_data += os.read(fd_w, 1024)
            print("Shield msg :",msg.decode('utf-8'))
        os.close(fp_r)
        os.close(fd_w)
        return r_data 
    def get_user_data(self):
        user_html = requests.get(self.user_url).text
        return user_html
    def get_access_token(self):
        access_token_html = requests.get(self.access_token_url).text
        return json.loads(access_token_html)["access_token"]
    def make_send_text(self,data):
        send_text = {
            "touser":[
            "ozAeU0m75lHBhw4ghsaQCL1jk3mg",
            "ozAeU0rK3uVkVc1dS3dBs2vpAVOQ"    
           ],
            "msgtype": "text",
            "text": { "content": data}
        }
        return send_text
    def send_text(self,data):
        send_text = self.make_send_text(data)
        send_text_json = json.dumps(send_text,ensure_ascii=False).encode("utf-8")
        request = requests.post(url = self.post_url,data = send_text_json).text
        print(request)
        return request


if __name__ == "__main__":
    s = Snd()
    datas = s.get_data()
    for data in datas:
        s.send_text(data)
