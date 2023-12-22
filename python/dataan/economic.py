import requests  
  
# 设置API密钥和请求参数  
url = "http://api.worldbank.org/v2/country?format=json"  
params = {  
    "per_page": 1000,  # 每页返回的最大记录数  
    "incomelevel": "high",  # 收入水平，可选值包括"low", "middle", "high"等  
    "source": "2"  # 数据来源，这里选择World Bank  
}  
  
# 发送请求并获取响应  
response = requests.get(url, params=params)  
data = response.json()  
  
# 解析响应数据并进行数据处理  
# 这里只打印前10个国家的名称和代码作为示例  
for i in range(10):  
    country = data[i]  
    country_name = country["name"]  
    country_code = country["code"]  
    print(country_name, country_code)