import requests
from bs4 import BeautifulSoup
import re
from lxml import etree
import json

class Match:
    def get_data():
        base_url = "https://csgo.5eplay.com/events"
        html = requests.get(base_url).text
        html_xpath = etree.HTML(html)
        result = []
        for i in range(1,11):
            r = []
            r += html_xpath.xpath( '//div[@id="J_EventsSessionWrap"]/div['+str(i)+']/p[1]/span[1]')
            r += html_xpath.xpath( '//div[@id="J_EventsSessionWrap"]/div['+str(i)+']/p[2]/a')
            r += html_xpath.xpath( '//div[@id="J_EventsSessionWrap"]/div['+str(i)+']/p[3]/span/a')
            r += html_xpath.xpath( '//div[@id="J_EventsSessionWrap"]/div['+str(i)+']/div/span')
            r += html_xpath.xpath( '//div[@id="J_EventsSessionWrap"]/div['+str(i)+']/p[4]/span/a')
            data = ""
            for c in r:
                c = re.sub(re.compile('\n'),"",c.text)
                data += c + '\t';
            result.append(data) 
        data = ""
        for r in result:
            data = data + r + "\n"
        return data
        


