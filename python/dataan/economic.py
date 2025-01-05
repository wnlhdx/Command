import wbdata
import pandas as pd
import datetime
import matplotlib.pyplot as plt
import seaborn as sns

# 设置日期范围
start_date = datetime.datetime(2000, 1, 1)
end_date = datetime.datetime(2020, 12, 31)

# 选择需要的经济指标（供需弹性和政府政策相关）
indicators = {
    'NY.GDP.MKTP.CD': 'GDP (current US$)',         # GDP (当前美元)
    'SP.POP.TOTL': 'Total Population',              # 总人口
    'NE.CON.TOTL.ZS': 'Consumption Expenditure (% of GDP)',  # 消费支出占GDP的比例
    'GC.XPN.TOTL.GD.ZS': 'Government Expenditure (% of GDP)',  # 政府支出占GDP比例
    'FP.CPI.TOTL.ZG': 'Inflation Rate (%)',         # 通货膨胀率
}

# 选择国家（例如：中国、美国、印度、巴西、俄罗斯）
countries = ['USA', 'CHN', 'IND', 'BRA', 'RUS']

# 从 World Bank API 获取数据
data = wbdata.get_dataframe(indicators, country=countries, data_date=(start_date, end_date), convert_date=True)

# 查看前几行数据
print(data.head())

# 查看数据中的缺失值
print(data.isnull().sum())

# 填充缺失值（以均值填充为例）
data = data.fillna(data.mean())

# 查看清理后的数据
print(data.head())

# 计算GDP与通货膨胀率的弹性（用百分比变化来近似）
data['GDP Growth Rate'] = data['GDP (current US$)'].pct_change() * 100
data['Inflation Growth Rate'] = data['Inflation Rate (%)'].pct_change() * 100

# 计算需求价格弹性
data['Demand Price Elasticity'] = data['GDP Growth Rate'] / data['Inflation Growth Rate']

# 绘制弹性变化趋势
plt.figure(figsize=(10, 6))
sns.lineplot(x=data.index, y=data['Demand Price Elasticity'], label='Demand Price Elasticity', color='r')
plt.title('Demand Price Elasticity over Time (2000-2020)')
plt.xlabel('Year')
plt.ylabel('Elasticity')
plt.legend()
plt.grid(True)
plt.show()

# 绘制政府支出占GDP比例与GDP的关系
plt.figure(figsize=(10, 6))
sns.lineplot(x=data.index, y=data['Government Expenditure (% of GDP)'], label='Government Expenditure (% of GDP)', color='m')
sns.lineplot(x=data.index, y=data['GDP (current US$)'], label='GDP (current US$)', color='b')
plt.title('Government Expenditure and GDP over Time (2000-2020)')
plt.xlabel('Year')
plt.ylabel('Value')
plt.legend()
plt.grid(True)
plt.show()
