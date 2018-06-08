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
import aiohttp
import asyncio
import multiprocessing as mp
from selenium.webdriver.chrome.options import Options
from selenium import webdriver
from lxml import etree
class Launcher:
    def __init__(self,username,password):
        self.username = username
        self.password = password
    def SetSession(self,session):
        self.session = session
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
        except:
            print("1st login errer")
            return None,None
        try:
            login_html = self.session.get(login_url,headers = headers).text
            login_url = p1.search(login_html).group(1)
        except:
            print("2nd jump errer")
            return None,None
        try:
            login_html = self.session.get(login_url,headers = headers).text
            login_url = 'https://weibo.com/u/' + p2.search(login_html).group(1)
        except:
            print("3rd jump errer")
            return None,None
        return login_url,request.cookies
    async def get_att_url(self,url,headers,cookies):
        try:
            async with aiohttp.ClientSession(cookies = cookies) as session:
                async with session.get(url,headers = headers) as r:
                    html =await r.text()
                    await asyncio.sleep(0.5)
                    return html
        except:
            return None
    async def get_some_att(self,url,headers,cookies):
        async with aiohttp.ClientSession(cookies = cookies) as session:
            async with session.get(url,headers = headers) as r:
                html =await r.text()
                await asyncio.sleep(0.5)
                return html

def analyse_some_att(html):
    p = re.compile(r'action-data=\\"uid=(\d{1,15})&nick=(.+?)\\">')
    soup = BeautifulSoup(html,'lxml')
    _title = soup.find("title")
    if _title:
        title = _title.get_text()
    else:
        title = _title
    att = p.findall(html)
    url_list = []
    for _id in att:
        url = 'https://weibo.com/u/' + _id[0] + '?from=myfollow_all'
        url_list.append(url)
        #print(url)
    url_list = set(url_list)
    return title,url_list

def analyse_att_url(html):
        p = re.compile(r'href=\\"(\\/\\/weibo.com(\\/p)?\\/\d+\\/follow\?from=page_\d+&wvr=6&mod=headfollow#place)\\"')
        try :
            url1 = p.search(html)
            url1 = url1.group(1)
            url2 = re.sub('/','',url1)
            soup = BeautifulSoup(html,'lxml')
            url = 'https:' + re.sub(re.compile(r'\\'),'/',url2)
            #print(url)
        except:
            print('search attaiton url error')
            return 'https://weibo.com/6562848155/follow?rightmod=1&wvr=6'
        return url

def analyse_some_msg(url,driver,cookies):
    driver.add_cookie(cookies)
    driver.get(url)
    time.sleep(1)
    html = driver.page_source
    print("\n\n\n\n\nddddddddddddddddddddddd\n\n\n\n\n")
    print(html)
    driver.delete_all_cookies()
    driver.close()

def init_browser():
    options = Options()
    options.add_argument("--headless")
    options.add_argument('--no-sandbox')
    options.add_argument('start-maximized')
    options.add_argument('window-size=1920x1080')
    options.add_argument("--mute-audio")
    options.add_argument('disable-infobars')
    options.add_argument("--disable-extensions")
    browser = webdriver.Chrome(executable_path = r'/usr/local/bin/chromedriver',options=options)
    return browser


async def main(loop):
    pool = mp.Pool(4)
    cookies_pool = []
    url = "https://weibo.com/u/3195299207?from=myfollow_all"
    count = 1
    base_url = 'https://weibo.com/'
    i = 1
    seen = set()
    unseen = set([base_url])
    browser = init_browser()
    while (len(cookies_pool) < 20) and (base_url is not None):
        user = Launcher('18545588767','19960419jh')
        session = requests.session()
        user.SetSession(session)
        base_url,cookies = user.login()
        print('get cookies No.',i)
        i += 1
        cookies_pool.append(cookies)
        time.sleep(1)
    print('cookies_pool len:',len(cookies_pool))
    analyse_some_msg(url,browser,random.sample(cookies_pool,1)[0])
    while True:  
        #async with aiohttp.ClientSession(cookies=random.sample(cookies_pool,1)[0] ) as session:
            #user.SetSession(session)
        while len(unseen) != 0:
            print('\nget  attation url ing ...')
            tasks = [loop.create_task(user.get_att_url(url,random.sample(headers_group,1)[0],random.sample(cookies_pool,1)[0])) for url in unseen]
            finished,unfinished = await asyncio.wait(tasks)
            htmls = [f.result() for f in finished]
            time.sleep(1)
            print('\nanalyse attation html ing ...')
            parse_jobs = [pool.apply_async(analyse_att_url,args=(html,)) for html in htmls]
            att_urls = [j.get() for j in parse_jobs]
            time.sleep(1)
            print('\nget some attation ing ...')
            tasks = [loop.create_task(user.get_some_att(url,random.sample(headers_group,1)[0],random.sample(cookies_pool,1)[0])) for url in att_urls]
            finished,unfinished = await asyncio.wait(tasks)
            htmls = [f.result() for f in finished]
            time.sleep(1)
            print('\nanalyse some attation ing ...')
            parse_jobss = [pool.apply_async(analyse_some_att,args=(html,)) for html in htmls]
            url_lists = [j.get() for j in parse_jobss]
            print('\nupdata seen ing ...')
            seen.update(unseen)
            unseen.clear()
            for title,url_list in url_lists:
                print(count,' ',title)
                unseen.update(url_list - seen)
                count += 1
            time.sleep(3)

            print('\nanalyse some message...')
            parse_jobsss = [pool.apply_async(analyse_some_msg,args=(url,browser,random.sample(cookies_pool,1)[0])) for url in unseen]
            print('use time:',time.time()-t1,'get user number:',count)


if __name__ == "__main__":
    file= open('headers','r')
    headers_group = []
    for headers in file.readlines():
        headers = {"User-Agent":headers.strip('\n')}
        headers_group.append(headers)
    t1 = time.time()
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main(loop))
    loop.close()
    
    
