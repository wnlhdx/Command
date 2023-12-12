# 开始一个新的脚本  
  
# 设置文件路径  
file_path = "testv.wav"  
  
# 读取音频文件  
sound = Read from file: file_path  
  
# 设置分析参数  
min_pitch = 75  # 最小音高（Hz）  
max_pitch = 600  # 最大音高（Hz）  
  
# 进行音高检测  
To Pitch (cc): sound, 0, 0, min_pitch, max_pitch  
Get number of pitch points: number_of_pitch_points  
Get mean: 0, 0, "Hertz", "parabolic", 0.03, 0.45  
  
# 输出结果  
printline("Mean pitch: " + mean)  
  
# 对元音进行分析  
for i from 1 to number_of_pitch_points do  
    Get point: i, pitch_time, pitch_frequency  
    if pitch_frequency >= min_pitch and pitch_frequency <= max_pitch then  
        # 在此范围内认为是元音，进行相应分析  
        printline("Vowel found at time: " + pitch_time + " with pitch: " + pitch_frequency)  
    endif  
endfor  
  
# 删除对象以释放内存  
RemoveObject: sound, pitch