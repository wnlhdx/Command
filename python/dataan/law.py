import requests  
from bs4 import BeautifulSoup  
import re  
  
def fetch_case_data(url):  
    response = requests.get(url)  
    soup = BeautifulSoup(response.text, 'lxml')  
      
    # 根据网站结构调整选择器  
    title_selector = 'div.case-title a'  
    detail_selector = 'div.case-detail'  
      
    # 获取案例标题和链接  
    titles = soup.select(title_selector)  
    links = [title.get('href') for title in titles]  
    titles = [title.text for title in titles]  
      
    # 获取案例详情  
    details = soup.select(detail_selector)  
    descriptions = [detail.text.strip() for detail in details]  
      
    # 返回数据  
    return zip(titles, links, descriptions)  
  
if __name__ == '__main__':  
    # 假设我们要抓取的是“民事”案例，这里只是一个示例URL，需要根据实际情况进行调整  
    url = 'http://wenshu.court.gov.cn/List/List?Type=民事'  
    results = fetch_case_data(url)  
    for title, link, description in results:  
        print(f"Title: {title}")  
        print(f"Link: {link}")  
        print(f"Description: {description}")  
        print("-" * 50)