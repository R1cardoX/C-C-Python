import requests
import random
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
import pandas as pd
import numpy as np
import binascii
import aiohttp
import asyncio
import multiprocessing as mp
class Scrapy:
    t0 = time.time()
    count = 0
    connect_count = 0
    login_url = "https://weibo.com"
    def __init__(self,time = 10):
        self.time = time
        self.user_pool = self.init_users()
        self.headers_pool = self.init_headers()
        self.cookies_pool = self.init_cookies()

    def init_headers(self):
        file = open('./res/headers','r')
        headers_group = []
        for headers in file.readlines():
            headers = {"User-Agent":headers.strip('\n')}
            headers_group.append(headers)
        return headers_group

    def init_cookies(self,times = 3):
        cookies_pool = []
        i = 0
        for _ in range(times):
            for user in self.user_pool:
                cookies = self.login(user)
                print('Get cookies No.',i+1,"with user:",user[0])
                cookies_pool.append(cookies)
                time.sleep(1)
                i += 1
        print('\n................................cookies_pool len:',len(cookies_pool))
        self.t0 = time.time()
        return cookies_pool
    
    def init_users(self):
        file = open('./res/userform','r')#格式 用户名:密码
        users_group = []
        for users in file.readlines():
            users = (users.strip('\n').split(':')[0],users.strip('\n').split(':')[1])
            users_group.append(users)
        return users_group

    def get_encrypted_name(self,user):
        username_urllike = urllib.request.quote(user[0])
        username_encrypted = base64.b64encode(bytes(username_urllike,encoding = 'utf-8'))
        return username_encrypted.decode('utf-8')

    def get_prelogin_args(self,user):
        '''
        模拟预登陆 获取服务器返回的 nonce ，servertime，pub_key 等信息
        '''
        json_pattern = re.compile('\((.*)\)')
        url = 'https://login.sina.com.cn/sso/prelogin.php?entry=weibo&callback=sinaSSOController.preloginCallBack&su=' + self.get_encrypted_name(user) + '&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.19)'
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

    def get_encrypted_pw(self,data,user):
        rsa_e = 65537
        pw_string = str(data['servertime']) + '\t' + str(data['nonce']) + '\n' +str(user[1])
        key = rsa.PublicKey(int(data['pubkey'],16),rsa_e)
        pw_encypted = rsa.encrypt(pw_string.encode('utf-8'),key)
        self.password = ''
        passwd = binascii.b2a_hex(pw_encypted)
        return passwd
        
    def build_post_data(self,raw,user):
        post_data = {
            "entry": "weibo",
            "gateway": "1",
            "from": "",
            "savestate": "7",
            "useticket": "1",
            "pagerefer": "https://login.sina.com.cn/crossdomain2.php?action=logout&r=https%3A%2F%2Fweibo.com%2Flogout.php%3Fbackurl%3D%252F",
            "vsnf": "1",
            "su": self.get_encrypted_name(user),
            "service": "miniblog",
            "servertime": raw['servertime'],
            "nonce": raw['nonce'],
            "pwencode": "rsa2",
            "rsakv": raw['rsakv'],
            "sp": self.get_encrypted_pw(raw,user),
            "sr": "1440*900",
            "encoding": "UTF-8",
            "prelt": "329",
            "url": "https://weibo.com/ajaxlogin.php?framelogin=1&callback=parent.sinaSSOController.feedBackUrlCallBack",
            "returntype": "META"
        }
        #data = urllib.parse.urlencode(post_data).encode('utf-8')
        return post_data

    def login(self,user):
        url = 'https://login.sina.com.cn/sso/login.php?client=ssologin.js(v1.4.19)'
        p = re.compile('location\.replace\(\"(.*?)\"\)')
        p1 = re.compile('location\.replace\(\'(.*?)\'\)')
        p2 = re.compile(r'"uniqueid":"(\d*?)"')
        headers = random.sample(self.headers_pool,1)[0]
        data = self.get_prelogin_args(user)
        post_data = self.build_post_data(data,user)
        try:
            request = requests.post(url = url,data = post_data,headers = headers)
            login_url = p.search(request.text).group(1)
        except:
            print("1st login errer")
            print("..............................Error html:\n",request)
            return None
        try:
            login_html = requests.get(login_url,headers = headers).text
            login_url = p1.search(login_html).group(1)
        except:
            print("2nd jump errer")
            print("..............................Error html:\n",login_html)
            return None
        try:
            login_html = requests.get(login_url,headers = headers).text
            login_url = 'https://weibo.com/u/' + p2.search(login_html).group(1)
        except:
            print("3rd jump errer")
            print("..............................Error html:\n",login_html)
            return None
        self.login_url = login_url
        return request.cookies

    async def connect_url(self,url):
        if url is None:
            return None
        self.count += 1
        print(self.count,".\tConnect url :",url,"\tuse time:",time.time()-self.t0)
        i = 2
        for _ in range(3):
            headers = random.sample(self.headers_pool,1)[0]
            cookies = random.sample(self.cookies_pool,1)[0]
            self.connect_count += 1
            try:
                async with aiohttp.ClientSession(cookies = cookies) as session:
                    async with session.get(url,headers = headers) as r:
                        html =await r.text()
                        await asyncio.sleep(self.time)
                        return html
            except:
                print("Connect url:",url,"\terror\tsleep 5s then retry in:",i,"\tall connect:",self.connect_count)
                i -= 1
                await asyncio.sleep(10)
        print("Wait for 60s then get cookies,url:",url)
        await asyncio.sleep(60)
        t1 = time.time() - self.t0
        cookies = self.init_cookies(1)
        self.t0 = t1
        self.connect_url(url)


def analyse_att_url(html):
    if html is None:
        return None
    html1 = re.sub(re.compile(r'\\/'),"/",html)
    html2 = re.sub(re.compile(r'\\"'),"\"",html1)
    html3 = re.sub(re.compile(r'\\t'),"\t",html2)
    html4 = re.sub(re.compile(r'\\n'),"\n",html3)
    html5 = re.sub(re.compile(r'\\r'),"\r",html4)
    p = re.compile('href="(/\d+/follow\?rightmod=\d&wvr=\d)"')
    p1 = re.compile(r'<span\s*class="item_ico\s*W_fl">\s*<em\s*class="W_ficon\s*ficon_cd_place\s*S_ficon">[^<>]*</em>[^<>]*</span>[^<>]*<span\s*class="item_text\s+W_fl">\s*([^<>\s]+)\s*[^<>\s]*\s*</span>') 
    p2 = re.compile(r'<title>([^<>]+)</title>')
    p3 = re.compile(r'href="//weibo.com(/p/\d+/follow\?from=page_\d+&wvr=6&mod=headfollow#place)"')
    try:
        title = p2.search(html5).group(1).split("的微博")[0]
    except:
        print("Search title error ...")
        return None,None
    try:
        match = p3.search(html5)
        if match is None:
            match = p.search(html5)
        url = "https://weibo.com" + match.group(1)
    except:
        print("Search url error\t\t\t\t\t\tError Title:",title)
        return None,None
    try:
        location = p1.search(html5).group(1)
    except:
        print("Search location error ...but get the url:",url,"\tError Title:",title)
        return url,None
    print("Get the url:",url,"\tfrom:",title,"\t\t\t\tLocation:",location)
    return url,location

def analyse_some_att(html):
    if html is None:
        return None
    html1 = re.sub(re.compile(r'\\/'),"/",html)
    html2 = re.sub(re.compile(r'\\"'),"\"",html1)
    html3 = re.sub(re.compile(r'\\t'),"\t",html2)
    html4 = re.sub(re.compile(r'\\n'),"\n",html3)
    html5 = re.sub(re.compile(r'\\r'),"\r",html4)
    p = re.compile(r'action-data=\\"uid=(\d{1,15})&nick=(.+?)\\">')
    p2 = re.compile(r'<title>([^<>]+)</title>')
    title = p2.search(html5).group(1).split("的微博")[0]
    try:
        att = p.findall(html)
        url_list = []
        for _id in att:
            url = 'https://weibo.com/u/' + _id[0]# + '?from=myfollow_all'
            url_list.append(url)
            print("Get the user url:",url,"\t\tfrom ",title)
    except:
        return None
    return set(url_list)


async def main(loop):
    pool = mp.Pool(4)
    url = "https://weibo.com/u/3195299207?from=myfollow_all"
    base_url = 'https://weibo.com/'
    seen = set()
    unseen = set([base_url])
    att_urls = set()
    user = Scrapy()
    while len(unseen) != 0 or len(att_urls) <= 10000:
        print('\nGet  attation url ing ...')
        tasks = [loop.create_task(user.connect_url(url)) for url in unseen]
        finished,unfinished = await asyncio.wait(tasks)
        htmls = [f.result() for f in finished]

        print('\nAnalyse attation html ing ...')
        parse_jobs = [pool.apply_async(analyse_att_url,args=(html,)) for html in htmls]
        user_datas = [j.get() for j in parse_jobs]
        locations = pd.Series([0],index = ['北京'])
        for url,location in user_datas:
            if url is None:
                continue
            elif url in att_urls:
                continue
            if location is not None:
                if location in locations:
                    locations[location] += 1
                else:
                    locations[location] = 1
            att_urls.add(url)
        print("\nLocations:\n",locations)
        locations.to_csv('./res/location.csv')

        print('\nGet some attation ing ...')
        tasks = [loop.create_task(user.connect_url(url)) for url in att_urls]
        finished,unfinished = await asyncio.wait(tasks)
        htmls = [f.result() for f in finished]

        print('\nAnalyse some attation ing ...')
        parse_jobss = [pool.apply_async(analyse_some_att,args=(html,)) for html in htmls]
        url_lists = [j.get() for j in parse_jobss]

        print('\nUpdata seen ing ...')
        seen.update(unseen)
        unseen.clear()

        for url_list in url_lists:
            unseen.update(url_list - seen)

        print('Use time:',time.time()-user.t0,'get user number:',len(seen)+len(unseen))
        user.cookies_pool = []
        user.cookies_pool = user.init_cookies()


if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main(loop))
    loop.close()
    
    

