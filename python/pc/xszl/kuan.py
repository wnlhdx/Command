from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.service import Service
from webdriver_manager.chrome import ChromeDriverManager
import time

# 初始化WebDriver
driver = webdriver.Chrome(service=Service(ChromeDriverManager().install()))

# 打开抖音登录页面
driver.get('https://www.douyin.com/')

# 登录操作（可能需要手动输入验证码）
# ...

# 打开关注列表页面
# 注意：这里需要根据实际情况替换为正确的URL
driver.get('https://www.douyin.com/your_following_list')

# 模拟滚动，加载更多关注用户
for _ in range(3):  # 模拟滚动3次，根据需要调整
    driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
    time.sleep(3)  # 等待页面加载

# 定位关注用户元素
# 注意：这里需要根据实际情况替换为正确的选择器
following_elements = driver.find_elements(By.CSS_SELECTOR, '.user-name')

# 提取前100个关注用户的名称
following_users = [user.text for user in following_elements[:100]]

# 打开播放量高的视频列表页面
# 注意：这里需要根据实际情况替换为正确的URL
driver.get('https://www.douyin.com/popular_videos')

# 模拟滚动，加载更多视频
for _ in range(3):  # 模拟滚动3次，根据需要调整
    driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
    time.sleep(3)  # 等待页面加载

# 定位视频播放量元素
# 注意：这里需要根据实际情况替换为正确的选择器
video_elements = driver.find_elements(By.CSS_SELECTOR, '.video-play-count')

# 提取前100个视频的播放量
video_play_counts = [int(play_count.text) for play_count in video_elements[:100]]

# 关闭浏览器
driver.quit()

# 打印结果
print(following_users)
print(video_play_counts)