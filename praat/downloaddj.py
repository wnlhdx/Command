
import requests  
from bs4 import BeautifulSoup  
from requests.adapters import HTTPAdapter  
from requests.packages.urllib3.util.retry import Retry  
import urllib
import os  


  
def get_mp3_links(url):  
    headers = {  
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'  
    }  # 使用合适的User-Agent，避免被检测为爬虫  
  
    session = requests.Session()  
    retry = Retry(total=5, backoff_factor=0.1, status_forcelist=[ 500, 502, 503, 504 ])  
    adapter = HTTPAdapter(max_retries=retry)  
    session.mount('http://', adapter)  
    session.mount('https://', adapter)  
    response = session.get(url, headers=headers, timeout=10)  # 这里的timeout参数表示请求超时的时
    soup = BeautifulSoup(response.content, 'html.parser')  
  
    mp3_links = []  
    base_links="https://dictionary.cambridge.org"
    audio_elements = soup.find_all('audio', class_='hdn')  # 修改为正确的选择器来定位音频元素  
    for audio in audio_elements:  
        mp3_links.append(base_links+audio.find('source', type='audio/mpeg')['src'])  # 提取MP3文件的URL  
        mp3_links.append(base_links+audio.find('source', type='audio/ogg')['src'])  # 提取OGG文件的URL（如果存在）  

    return mp3_links  
  
def download_mp3(links, output_dir):  
    if not os.path.exists(output_dir):  
        os.makedirs(output_dir)  
  
    print("输出路径"+output_dir)
    for link in links:  
        filename = link.split('/')[-1]  # 提取文件名  
        headers = {  
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'  
        }  #
        request = urllib.request.Request(url, headers=headers)  # 设置超时时间为10秒  
        with urllib.request.urlopen(request, timeout=10) as response:  
            # 读取数据  
            data = response.read()  
            with open('dj/'+filename, 'wb') as file:  
                file.write(data)
                print(filename+"下载完成") 
  
if __name__ == '__main__':  
    url = 'https://dictionary.cambridge.org/help/phonetics.html'  # 替换为你要爬取的网页URL  
    output_dir = 'dj'  # 下载的MP3文件保存路径，可以根据需要更改  
    mp3_links = get_mp3_links(url)  
    download_mp3(mp3_links, output_dir)