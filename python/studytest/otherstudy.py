import math

import numpy as np  # 导入科学计算库
import matplotlib.pyplot as plt  # 导入绘图库
from scipy import signal

a = np.arange(0, 2000, 1)
b = 1 / (np.sqrt(1 + ((a-500)*(a-500)) * ((math.sqrt(2) - 1) / 625)))
c = 1 / (np.sqrt(1 + ((a-1500)*(a-1500)) * ((math.sqrt(2) - 1) / 5625)))
plt.plot(a, b*c, 'r--')
plt.xlabel('横轴：频率', fontproperties='SimHei', fontsize=15, color='green')  # 标注x轴标签
plt.ylabel('纵轴：振幅', fontproperties='SimHei', fontsize=15)  # 标注y轴标签
plt.title('带通滤波器', fontproperties='SimHei', fontsize=25)
# plt.text(400,5,'y=sinx',fontsize=15)
plt.axis([0, 2000, -1, 1])  # 确定x,y轴的区域范围
# xz=np.arange(0,0.1,0.01)
# plt.xticks(xz)
plt.grid(True)
plt.show()
