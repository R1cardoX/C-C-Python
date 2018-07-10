import re
from jieba import analyse
import requests
from bs4 import BeautifulSoup
import sys
from lxml import etree
class Match:
    def get_data():
        base_url = "https://m.ithome.com/"
        html = requests.get(base_url).text
        soup = BeautifulSoup(html,"lxml")
        r = soup.find_all('p',{"class":"plc-title","role":"heading"})
        data = "24小时日榜\n"
        for i in r:
            data += i.get_text() + "\n"
        return data

    def get_key():
        textrank = analyse.textrank
        base_url = "https://m.ithome.com/"
        html = requests.get(base_url).text
        rr = soup.find('a',{"href":re.compile("https://m.ithome.com/html/\d+.htm")})
        html = requests.get(rr['href']).text
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

