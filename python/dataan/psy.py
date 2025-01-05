import pandas as pd
import statsmodels.api as sm
import seaborn as sns
import matplotlib.pyplot as plt
from scipy import stats
import os

def get_data_path():
    path = ""
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        path += "~/"
    elif os.name == 'nt':
        # Windows 系统
        path += "D:/work/data/"
    path+="big5data.csv"
    return path

# 加载数据
data = pd.read_csv(get_data_path(), sep='\t')
print(data.columns)
# 1. 数据预处理
# 删除缺失数据
data = data.dropna()

# 2. 观察者偏见分析
# 分析性别与BIG5维度的关系
gender_big5 = data[['gender', 'E1', 'N1', 'A1', 'C1', 'O1']].copy()

# 将性别变量转化为类别变量
gender_big5['gender'] = gender_big5['gender'].map({1: 'Male', 2: 'Female', 3: 'Other'})

# 使用箱线图查看性别与BIG5维度的关系
plt.figure(figsize=(12, 6))
sns.boxplot(x='gender', y='E1', data=gender_big5)
plt.title('Gender and Extraversion (E1)')
plt.show()

# 进行方差分析（ANOVA）检验性别与外向性得分之间的关系
anova_result = stats.f_oneway(gender_big5[gender_big5['gender'] == 'Male']['E1'],
                               gender_big5[gender_big5['gender'] == 'Female']['E1'],
                               gender_big5[gender_big5['gender'] == 'Other']['E1'])

print(f'ANOVA result for E1 (Extraversion) and gender: F={anova_result.statistic}, p={anova_result.pvalue}')

# 3. 操作定义检验
# 确认每个BIG5维度的条目分数的一致性
big5_columns = ['E1', 'E2', 'E3', 'E4', 'E5', 'N1', 'N2', 'N3', 'A1', 'C1', 'O1']
big5_data = data[big5_columns]

# 查看每个维度的相关性
correlation_matrix = big5_data.corr()
sns.heatmap(correlation_matrix, annot=True, cmap='coolwarm', fmt='.2f')
plt.title('Correlation Matrix for BIG5 Items')
plt.show()

# 4. 代替解释和控制需求的分析
# 使用回归分析控制潜在的干扰变量（例如，性别、年龄、种族）
X = data[['gender', 'age', 'race', 'engnat']]
X = pd.get_dummies(X, drop_first=True)  # 将分类变量转换为虚拟变量
X = sm.add_constant(X)  # 添加常数项

# 选择一个BIG5维度（例如，外向性E1）
y = data['E1']

# 执行线性回归
model = sm.OLS(y, X).fit()
print(model.summary())

# 通过回归分析，我们可以判断性别、年龄、种族等变量是否显著影响外向性得分（E1），并进一步控制这些因素。
