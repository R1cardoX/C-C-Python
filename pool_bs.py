import multiprocessing as mp
import time
from urllib.request import urlopen, urljoin
from bs4 import BeautifulSoup
import re
import requests
#re_comple = r'/galler(ies(\?offset=\d{1,3})?)?(y/\d+/.+)?$'
#base_url = r'https://www.hltv.org'
re_comple = r'/galler(ies(\?offset=\d{1,3})?)?(y/\d+/.+)?$'
base_url = r'https://www.hltv.org'
def crawl(url):
    response = requests.get(url).text
    time.sleep(0.5)
    return response

def parse(html):
    soup = BeautifulSoup(html,'lxml')
    urls = soup.find_all('a',{"href":re.compile(re_comple)})
    title = soup.find('title').get_text().strip()
    page_urls = set([urljoin(base_url,url['href']) for url in urls])
    _url = soup.find_all('meta',{'property':"og:url"})
    if len(_url):
        url = _url[0]['content']
    else:
        url = None
    return title,page_urls,url
class Findurls:
    seen = set()
    count = 0

    def __init__(self,url):
        self.base_url = url
        self.pool = mp.Pool(4)
        self.unseen = set([url,])


    def get_some_urls(self):
        while len(self.unseen)!=0:
            if len(self.seen) > 500:
                break
            print('\nDistributed Crawling ...')
            crawl_jobs = [self.pool.apply_async(crawl,args=(url,)) for url in self.unseen]
            htmls = [j.get() for j in crawl_jobs]
            print('\nDistributed Parsing...')
            parse_jobs = [self.pool.apply_async(parse,args=(html,)) for html in htmls]
            results = [j.get() for j in parse_jobs]
            print('\nAnalysing...')
            self.seen.update(self.unseen)
            self.unseen.clear()
            for title,page_urls,url in results:
                print(self.count,' ',title,' ',url)
                self.count += 1
                self.unseen.update(page_urls - self.seen)

if __name__ == '__main__':
    find = Findurls(base_url)
    find.get_some_urls()
