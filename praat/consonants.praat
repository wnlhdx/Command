# 开始一个新的脚本  
  
# 设置文件路径  
file_path = "consonants/con-00a.wav"  
  
# 读取音频文件  
sound = Read from file: file_path  
To Spectrum: sound, 0, 0, "YES", "YES", 50, 500  
Get number of spectrum points: number_of_spectrum_points  
Get mean: 0, 0, "Hertz", "parabolic", 0.03, 0.45  
  
# 输出结果  
printline("Mean spectrum: " + mean)  
  
# 对辅音进行分析，通常辅音在频谱上有特定表现  
for i from 1 to number_of_spectrum_points do  
    Get point: i, spectrum_time, spectrum_intensity  
    if spectrum_intensity > threshold then # 设置一个合适的阈值来检测辅音  
        printline("Consonant found at time: " + spectrum_time + " with intensity: " + spectrum_intensity)  
    endif  
endfor  
  
# 删除对象以释放内存  
RemoveObject: sound, spectrum
