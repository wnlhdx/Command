import pandas as pd
import os
def get_data_path():
    path = ""
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        path += "~/"
    elif os.name == 'nt':
        # Windows 系统
        path += "D:/work/data/"
    path+="WVS_Time_Series_1981-2022_csv_v5_0.csv"
    return path

# 加载数据集
data = pd.read_csv(get_data_path())



# 查看数据基本信息
print(data.head())          # 查看前几行
print(data.info())          # 查看字段信息
print(data.describe())      # 查看数值字段的基本统计信息
