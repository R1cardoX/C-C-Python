# -*- coding: UTF-8 -*-
import requests
import json
from five_e import Match
APPID = "wx8368c8bd768f22e1"
appsecret = "2e43382589842298b984c6b871851c8d"
access_token_url = "https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=" +APPID + "&secret=" + appsecret
access_token_html = requests.get(access_token_url).text
access_token = json.loads(access_token_html)["access_token"]
post_url = "https://api.weixin.qq.com/cgi-bin/message/mass/send?access_token=" + access_token
user_url = "https://api.weixin.qq.com/cgi-bin/user/get?access_token=" + access_token + "&next_openid="
user_html = requests.get(user_url).text
print(user_html)
data  = Match.get_data()
print(data)
send_text = {
    "touser":[
    "ozAeU0m75lHBhw4ghsaQCL1jk3mg",
    "ozAeU0rK3uVkVc1dS3dBs2vpAVOQ"    
   ],
    "msgtype": "text",
    "text": { "content": data}
}
send_text_json = json.dumps(send_text,ensure_ascii=False).encode("utf-8")
request = requests.post(url = post_url,data = send_text_json).text
print(request)
