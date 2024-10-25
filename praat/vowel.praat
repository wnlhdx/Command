# Praat 脚本示例

# 从外部提取语音文件
file_standard$="standard/vowels/vow-00a.wav"
file_user$="my/vowels/1.wav"

# 加载语音文件
Read from file: file_standard$

Read from file: file_user$

# 提取共振峰信息
selectObject: "Sound 1"
To Formant (burg): 0, 2, 5000, 0.025, 50 

selectObject: "Sound vow-00a"
To Formant (burg): 0, 2, 5000, 0.025,  50 

# 获取共振峰的频率和强度
# 使用 Get value in Table 获取共振峰的频率和强度
selectObject: "Formant vow-00a"
standard_f1_freq = Get value in Table: 1, 1
standard_f2_freq = Get value in Table: 2, 1
standard_f1_intensity = Get value in Table: 1, 2
standard_f2_intensity = Get value in Table: 2, 2


selectObject: "Formant 1"
user_f1_freq = Get value in Table: 1, 1
user_f2_freq = Get value in Table: 2, 1
user_f1_intensity = Get value in Table: 1, 2
user_f2_intensity = Get value in Table: 2, 2

# 计算中心频率的加权平均值
weighted_average_freq = (standard_f1_freq * standard_f1_intensity + standard_f2_freq * standard_f2_intensity) / (standard_f1_intensity + standard_f2_intensity)

# 计算差异百分比
f1_freq_difference = ((user_f1_freq - standard_f1_freq) / standard_f1_freq) * 100
f2_freq_difference = ((user_f2_freq - standard_f2_freq) / standard_f2_freq) * 100

# 反馈给用户
selectObject: 1
if weighted_average_freq > standard_weighted_average_freq
    comment("舌位过前" + f1_freq_difference + "%")
else
    comment("舌位过后" + abs(f1_freq_difference) + "%")
endif

if user_f1_intensity > standard_f1_intensity
    comment("唇过圆" + f1_intensity_difference + "%")
else
    comment("唇过展" + abs(f1_intensity_difference) + "%")
endif

if user_f2_freq > standard_f2_freq
    comment("舌位过高" + f2_freq_difference + "%")
else
    comment("舌位过低" + abs(f2_freq_difference) + "%")
endif
