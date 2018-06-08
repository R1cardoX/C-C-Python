import aiohttp
import asyncio
import time
import re
from bs4 import BeautifulSoup
import requests
import multiprocessing as mp
from urllib.request import urljoin
base_url = r'https://www.ithome.com/'

seen = set()
unseen = set([base_url])

def parse(html):
    soup = BeautifulSoup(html,'lxml')
    urls = soup.find_all('a',{"href":re.compile(r'//www\.ithome\.com/html/\w+/\d+\.htm')})
    title = soup.find('title').get_text().strip()
    page_urls = set([urljoin(base_url,url['href']) for url in urls])
    return title,page_urls

async def crawl(url,session):
    print('|',end = "")
    r = await session.get(url)
    html = await r.text()
    await asyncio.sleep(0.5)
    time.sleep(0.5)
    return html

async def main(loop):
    pool = mp.Pool(4)
    async with aiohttp.ClientSession() as session:
        count = 1
        while len(unseen) != 0:
            print('\nAsync Crawling...')
            tasks = [loop.create_task(crawl(url,session)) for url in unseen]
            finished, unfinished = await asyncio.wait(tasks)
            htmls = [f.result() for f in finished]

            print('\nDistributed Parsing...')
            parse_jobs = [pool.apply_async(parse,args=(html,)) for html in htmls]
            results = [j.get() for j in parse_jobs]

            print('\nAnalysing...')
            seen.update(unseen)
            unseen.clear()
            for title,page_urls in results:
                print(count,title)
                unseen.update(page_urls - seen)
                count += 1

if __name__ == "__main__":
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main(loop))
    loop.close()

