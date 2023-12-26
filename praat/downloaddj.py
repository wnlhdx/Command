
复制代码
import requests  
from bs4 import BeautifulSoup  
import os  
  
def get_mp3_links(url):  
    headers = {  
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'  
    }  # 使用合适的User-Agent，避免被检测为爬虫  
  
    response = requests.get(url, headers=headers)  
    soup = BeautifulSoup(response.content, 'html.parser')  
  
    mp3_links = []  
    audio_elements = soup.find_all('audio', class_='hdn')  # 修改为正确的选择器来定位音频元素  
    for audio in audio_elements:  
        mp3_links.append(audio.find('source', type='audio/mpeg')['src'])  # 提取MP3文件的URL  
        mp3_links.append(audio.find('source', type='audio/ogg')['src'])  # 提取OGG文件的URL（如果存在）  
  
    return mp3_links  
  
def download_mp3(links, output_dir):  
    if not os.path.exists(output_dir):  
        os.makedirs(output_dir)  
  
    for link in links:  
        filename = link.split('/')[-1]  # 提取文件名  
        with requests.get(link, stream=True) as r:  
            with open(os.path.join(output_dir, filename), 'wb') as f:  
                for chunk in r.iter_content(chunk_size=8192):  
                    if chunk:  
                        f.write(chunk)  
        print(f"Downloaded: {filename}")  
  
if __name__ == '__main__':  
    url = 'https://dictionary.cambridge.org/help/phonetics.html'  # 替换为你要爬取的网页URL  
    output_dir = 'dj'  # 下载的MP3文件保存路径，可以根据需要更改  
    mp3_links = get_mp3_links(url)  
    download_mp3(mp3_links, output_dir)