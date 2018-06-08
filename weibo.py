import requests
import random
from selenium import webdriver
import time
from bs4 import BeautifulSoup
import urllib.error
import urllib.parse
import urllib.request
import re
import rsa
import http.cookiejar
import base64
import urllib
import json
import binascii

class Launcher:
    def __init__(self,username,password):
        self.username = username
        self.password = password
        self.session = requests.session()
    def get_encrypted_name(self):
        username_urllike = urllib.request.quote(self.username)
        username_encrypted = base64.b64encode(bytes(username_urllike,encoding = 'utf-8'))
        return username_encrypted.decode('utf-8')
    def get_prelogin_args(self):
        '''
        模拟预登陆 获取服务器返回的 nonce ，servertime，pub_key 等信息
        '''
        json_pattern = re.compile('\((.*)\)')
        url = 'https://login.sina.com.cn/sso/prelogin.php?entry=weibo&callback=sinaSSOController.preloginCallBack&su=' + self.get_encrypted_name() + '&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.19)'
        try:
            request = urllib.request.Request(url)
            response = urllib.request.urlopen(request)
            raw_data = response.read().decode('utf-8')
            json_data = json_pattern.search(raw_data).group(1)
            data = json.loads(json_data)
            return data
        except urllib.error as e:
            print("%d"%e.code)
            return None
    def get_encrypted_pw(self,data):
        rsa_e = 65537
        pw_string = str(data['servertime']) + '\t' + str(data['nonce']) + '\n' +str(self.password)
        key = rsa.PublicKey(int(data['pubkey'],16),rsa_e)
        pw_encypted = rsa.encrypt(pw_string.encode('utf-8'),key)
        self.password = ''
        passwd = binascii.b2a_hex(pw_encypted)
        return passwd
    def enableCookies(self):
        cookie_container = http.cookiejar.CookieJar()
        cookie_support = urllib.request.HTTPCookieProcessor(cookie_container)
        opener = urllib.request.build_opener(cookie_support,urllib.request.HTTPHandler)
        urllib.request.install_opener(opener)
        
    def build_post_data(self,raw):
        post_data = {
            "entry": "weibo",
            "gateway": "1",
            "from": "",
            "savestate": "7",
            "useticket": "1",
            "pagerefer": "https://login.sina.com.cn/crossdomain2.php?action=logout&r=https%3A%2F%2Fweibo.com%2Flogout.php%3Fbackurl%3D%252F",
            "vsnf": "1",
            "su": self.get_encrypted_name(),
            "service": "miniblog",
            "servertime": raw['servertime'],
            "nonce": raw['nonce'],
            "pwencode": "rsa2",
            "rsakv": raw['rsakv'],
            "sp": self.get_encrypted_pw(raw),
            "sr": "1440*900",
            "encoding": "UTF-8",
            "prelt": "329",
            "url": "https://weibo.com/ajaxlogin.php?framelogin=1&callback=parent.sinaSSOController.feedBackUrlCallBack",
            "returntype": "META"
        }
        #data = urllib.parse.urlencode(post_data).encode('utf-8')
        return post_data
    def login(self):
        url = 'https://login.sina.com.cn/sso/login.php?client=ssologin.js(v1.4.19)'
        p = re.compile('location\.replace\(\"(.*?)\"\)')
        p1 = re.compile('location\.replace\(\'(.*?)\'\)')
        p2 = re.compile(r'"uniqueid":"(\d*?)"')
        self.enableCookies()
        data = self.get_prelogin_args()
        post_data = self.build_post_data(data)
        headers = {
            "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36"}
        try:
            request = self.session.post(url = url,data = post_data,headers = headers)
            login_url = p.search(request.text).group(1)
            login_html = self.session.get(login_url,headers = headers).text
            login_url = p1.search(login_html).group(1)
            login_html = self.session.get(login_url,headers = headers).text
            login_url = 'https://weibo.com/u/' + p2.search(login_html).group(1)
            return login_url
        except:
            print("errer")
    def get_att_url(self,url):
        headers = {
            "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36"}
        p = re.compile(r'href=\\"(\\/\\/weibo.com(\\/p)?\\/\d+\\/follow\?from=page_\d+&wvr=6&mod=headfollow#place)\\"')
        try:
            html = self.session.get(url,headers = headers).text
            url1 = p.search(html).group(1)
            url2 = re.sub('/','',url1)
            url = 'https:' + re.sub(re.compile(r'\\'),'/',url2)
            return url
        except:
            return None
    def get_some_att(self,url):
        headers = {
            "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36"}
        html = self.session.get(url,headers = headers).text 
        p = re.compile(r'action-data=\\"uid=(\d{1,15})&nick=(.+?)\\">')
        att = p.findall(html)
        url_list = []
        for _id in att:
            url = 'https://weibo.com/u/' + _id[0] + '?from=myfollow_all'
            url_list.append(url)
        return url_list
if __name__ == "__main__":
    user = Launcher('18545588767','19960419jh')
    url = user.login()
    print(url)
    url = user.get_att_url(url)
    print("关  注：",url)
    while True:
        url_list= user.get_some_att(url)
        url = random.sample(url_list,1)[0]
        print("关注人:",url)
        url = user.get_att_url(url)
        while url == None:
            url = random.sample(url_list,1)[0]
            print("关注人:",url)
            url = user.get_att_url(url)
            time.sleep(0.5)
        print("关  注：",url)
        time.sleep(0.5)

