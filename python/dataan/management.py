import requests  
import pandas

  
# Alpha Vantage API密钥  
api_key = 'K9W2WNJ0956KMGK9'  
  
# 发送HTTP请求获取行情数据  
url = 'https://www.alphavantage.co/query?function=TOP_GAINERS_LOSERS&apikey={api_key}'  
response = requests.get(url)  
data = response.json()
print(data)

tickers = [gainer['ticker'] for gainer in data['top_gainers']]  
for ticker in tickers[:3]:
  url2 ="https://www.alphavantage.co/query?function=OVERVIEW&symbol={ticker}&apikey={api_key}"
  response = requests.get(url)  
  data = response.json()
  print(data)
  df = pandas.DataFrame(data)
  df.to_csv('data.csv', index=False)


