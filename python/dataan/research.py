import os
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from kaggle.api.kaggle_api_extended import KaggleApi

# 设置Kaggle API密钥
api = KaggleApi()
api.authenticate()

# 下载数据集 (Kaggle 数据集 ID)
dataset_name = 'sunnykakar/oceanic-plastic-additives-concentration-dataset'  # 你选择的数据集ID
download_path = './kaggle'

# 下载数据集
api.dataset_download_files(dataset_name, path=download_path, unzip=True)

# 加载数据
file_path = './kaggle/Database Plastic Additives in Ocean 2024-02-22.xlsx'  # 假设数据集中的文件名是plastics_data.csv
data = pd.read_excel(file_path, sheet_name=None)

# 查看所有表格的名称
print(data.keys())

# 假设数据在一个名为"Compiled Data"的表格中，加载该表格
df = data['Compiled Data']

# 查看数据的前几行
print(df.head())

# 查看数据的列
print(df.columns)

# 检查缺失值
missing_values = df.isnull().sum()
print("缺失值统计：")
print(missing_values)

# 删除缺失值较多的列（如果需要）
df_cleaned = df.dropna(axis=1, thresh=int(len(df)*0.8))  # 保留至少80%非空数据的列

# 填补缺失值（如需要）
df_cleaned['Concentration'].fillna(df_cleaned['Concentration'].mean(), inplace=True)

# 查看清理后的数据
print(df_cleaned.head())

# 选择你感兴趣的列
additive_columns = ['Full Name of Compound', 'Concentration', 'Compartment']

# 可视化不同添加剂的浓度分布
plt.figure(figsize=(12, 6))
sns.boxplot(x='Full Name of Compound', y='Concentration', data=df_cleaned)
plt.xticks(rotation=90)
plt.title("Distribution of Plastic Additives Concentration")
plt.show()

# 分析不同样本类型（Compartment）中的添加剂浓度差异
plt.figure(figsize=(12, 6))
sns.boxplot(x='Compartment', y='Concentration', data=df_cleaned)
plt.title("Plastic Additives Concentration by Sample Compartment")
plt.show()

# 假设数据集中生物体相关的信息在 'Biota Sampling Type' 列中
biota_data = df_cleaned[df_cleaned['Biota Sampling Type'].notnull()]

# 查看不同生物体类型中的添加剂浓度
plt.figure(figsize=(12, 6))
sns.boxplot(x='Biota Sampling Type', y='Concentration', data=biota_data)
plt.xticks(rotation=90)
plt.title("Plastic Additives Concentration in Biota Samples")
plt.show()

# 选择特定的有害添加剂进行分析
harmful_additives = ['Full Name of Compound']
toxic_additives = ['BPA', 'Phthalates', 'Flame Retardants']  # 假设这些是有害添加剂的名称
toxic_data = df_cleaned[df_cleaned['Full Name of Compound'].isin(toxic_additives)]

# 可视化有害添加剂在不同生物体中的浓度
plt.figure(figsize=(12, 6))
sns.boxplot(x='Biota Sampling Type', y='Concentration', data=toxic_data)
plt.title("Concentration of Toxic Plastic Additives in Biota")
plt.show()
