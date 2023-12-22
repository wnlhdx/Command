import requests  
import json  
import matplotlib.pyplot as plt  
from datetime import datetime  
  
# 设置Wikipedia API的URL和参数  
api_url = "https://en.wikipedia.org/w/api.php"  
params = {  
    "action": "query",  
    "format": "json",  
    "list": "search",  
    "srsearch": "Prehistory",  # 搜索史前时代相关的页面  
    "srlimit": "50"  # 限制返回结果数量  
}  
  
# 发送请求并获取响应  
response = requests.get(api_url, params=params)  
data = response.json()  
  
# 解析响应数据，提取相关事件信息  
events = []  
for page in data["query"]["search"]:  
    page_id = page["pageid"]  
    page_title = page["title"]  
    params = {  
        "action": "query",  
        "format": "json",  
        "prop": "extracts",  
        "pageids": page_id,  
        "exintro": 1,  # 只提取页面引言部分  
        "explaintext": 1  # 以纯文本形式提取页面内容  
    }  
    page_response = requests.get(api_url, params=params)  
    page_data = page_response.json()  
    extract = page_data["query"]["pages"][str(page_id)]["extract"]  
    events.append((page_title, extract))  
  
# 清洗和处理事件数据，提取时间点和描述信息  
dates = []  
descriptions = []  
for event in events:  
    title, description = event  
    date = None  
    for line in description.split("\n"):  
        if "c." in line or "BC" in line or "BCE" in line:  # 根据行中的时间标注提取时间点  
            date = datetime.strptime(line, "%B %d, %Y")  # 假设日期格式为“Month day, year”  
            break  
    if date is not None:  
        dates.append(date)  
        descriptions.append(title)  
  
# 使用matplotlib库制作时间轴  
fig, ax = plt.subplots()  
ax.set_xlabel('Date')  
ax.set_ylabel('Event')  
ax.set_yticks([])  # 不显示y轴刻度  
ax.set_xlim([min(date for date in dates), max(date for date in dates)])  
ax.grid(True)  
  
# 绘制事件标记和描述信息  
for date, desc in zip(dates, descriptions):  
    ax.annotate(desc, (date, 0), textcoords="offset points", xytext=(0, 3), ha='center')  
  
# 保存图片  
plt.savefig('history.png')