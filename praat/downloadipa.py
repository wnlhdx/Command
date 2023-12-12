import os  
import requests  
from bs4 import BeautifulSoup  
from pydub import AudioSegment  
  
# 定义URL和对应的文件夹字典  
url_dict = {  
    "https://phonetics.ucla.edu/course/chapter1/consonants1.html": "consonants",  
    "https://phonetics.ucla.edu/course/chapter1/consonants2.html": "consonants",  
    "https://phonetics.ucla.edu/course/chapter1/vowels.html": "vowels",  
    "https://phonetics.ucla.edu/course/chapter1/clicks.html": "click",  
    "https://phonetics.ucla.edu/course/chapter1/others.html": "others",  
}  
  
# 遍历字典，对每个URL进行处理  
for url, folder_name in url_dict.items():  
    # 发送 HTTP 请求获取网页内容  
    response = requests.get(url)  
    html_content = response.text  
  
    # 使用 BeautifulSoup 解析 HTML 内容  
    soup = BeautifulSoup(html_content, "html.parser")  
  
    # 查找所有的链接标签<area>，并筛选出以 ".aiff" 结尾的链接  
    aiff_links = [link["href"] for link in soup.find_all("area") if link["href"].endswith(".aiff") or link["href"].endswith(".AIFF")]  
  
    # 创建保存 aiff 文件的目录  
    save_dir = folder_name  
    if not os.path.exists(save_dir):  
        os.makedirs(save_dir)  
        
    save_url="https://phonetics.ucla.edu/course/chapter1/"
  
    # 下载并保存 aiff 文件，并转换为 wav 格式  
    for aiff_link in aiff_links:  
        aiff_url = save_url + aiff_link  # 构造完整的 aiff 文件链接
        aiff_link=aiff_link.replace("ipaSOUNDS/","").lower()  
        aiff_filename = os.path.join(save_dir, aiff_link)  # 构造保存文件的路径  
        wav_filename = os.path.join(save_dir, aiff_link.replace(".aiff", ".wav"))  # 构造保存 wav 文件的路径  
  
        # 发送 HTTP 请求下载 aiff 文件  
        if not os.path.exists(aiff_filename): 
          aiff_response = requests.get(aiff_url) 
          with open(aiff_filename, "wb") as f:  
             f.write(aiff_response.content)  
             print(f"Downloaded: {aiff_filename}")  
        

        # 转换 aiff 文件为 wav 格式
        if not os.path.exists(wav_filename):  
          sound = AudioSegment.from_file(aiff_filename,format="aiff")  
          sound.export(wav_filename, format="wav")  
          print(f"Converted to WAV: {wav_filename}")  
  
print("All aiff files downloaded and converted to WAV.")