from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time
import appium
import pyautogui
import os

from webdriver_manager.chrome import ChromeDriverManager

# 初始化 WebDriver
service = Service(ChromeDriverManager().install())
driver = webdriver.Chrome(service=service)
driver.maximize_window()

try:
    # 打开 YouTube
    driver.get("https://www.youtube.com")

    # 等待页面加载完成并查找搜索框
    search_box = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.NAME, "search_query"))
    )

    # 在搜索框中输入关键词并回车
    search_query = "Python tutorial"
    search_box.send_keys(search_query)
    search_box.send_keys(Keys.RETURN)

    # 等待搜索结果加载
    video = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.ID, "video-title"))
    )

    # 点击第一个视频
    video.click()

    # 等待视频页面加载
    WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.CLASS_NAME, "html5-video-player"))
    )

    # 检查视频是否开始播放（通过视频时间变化来检测）
    video_player = driver.find_element(By.CLASS_NAME, "html5-video-player")
    time.sleep(5)  # 等待几秒钟查看视频是否开始播放
    current_time = video_player.get_attribute("currentTime")

    # 打印结果
    if float(current_time) > 0:
        print("视频成功播放！")
    else:
        print("视频未能播放。")

finally:
    # 关闭浏览器
    driver.quit()
