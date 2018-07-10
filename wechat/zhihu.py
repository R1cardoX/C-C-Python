import re
from jieba import analyse
import requests
from bs4 import BeautifulSoup

class Match:
    def get_data(self):
        base_url = "https://www.zhihu.com"
        headers = {"User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36"}
        html = requests.get(base_url,headers =headers).text
        soup = BeautifulSoup(html,"lxml")
        results = soup.find_all('a',{"data-za-detail-view-element_name":"Title","href":re.compile("/question/\d+/answer/\d+")})
        datas = "知乎\n"
        for i in results:
            datas += i.get_text() + " " +base_url+i["href"] + "\n"
        self.key_url = base_url + results[0]['href']
        return datas
    def get_key(self):
        textrank = analyse.textrank
        html = requests.get(self.key_url,headers = headers).text
        soup = BeautifulSoup(html,"lxml")
        rrr = soup.find_all('p')
        text = ""
        for i in rrr:
            text += i.get_text() + "\n"
        keywords = textrank(text)
        key = ""
        for keyword in keywords:
            key += keyword + " " 
        return key
