from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
import time
import os

from webdriver_manager.chrome import ChromeDriverManager

# 初始化 WebDriver
# driver_path = "./"
# if os.name == 'posix':
#     # Unix-like 系统，例如 Termux
#     driver_path += "driver-linux/chromedriver"
# elif os.name == 'nt':
#     # Windows 系统
#     driver_path += "driver-win/chromedriver.exe"
# if not os.path.exists(driver_path):
driver_path=ChromeDriverManager().install()
# 使用 Service 来指定 chromedriver 路径
service = Service(driver_path)
driver=webdriver.Chrome(service=service)
# 访问 Tubi 网站
driver.get("https://tubitv.com/")

# 等待页面加载
time.sleep(3)

# 定义视频类型集合
video_categories = ["Movies", "TV Shows", "Anime", "Documentaries"]  # 你可以根据 Tubi 网站实际视频分类更新

# 逐个测试视频类型
for category in video_categories:
    try:
        # 找到并点击视频类别（假设类别元素的 id、class 或 xpath 已知，下面的仅为示例）
        category_button = driver.find_element(By.XPATH, f"//span[contains(text(), '{category}')]")  # 通过文本查找类别
        category_button.click()

        # 等待页面加载
        time.sleep(3)

        # 获取当前页面中的视频列表，假设视频名称显示在某个 class 中（根据页面实际情况调整）
        video_elements = driver.find_elements(By.CLASS_NAME, "video-item")  # 替换为实际的视频元素 class

        # 验证视频类型是否符合
        print(f"正在验证 {category} 类别的视频：")
        for video in video_elements:
            video_title = video.text  # 获取视频标题
            print(f"视频标题: {video_title}")

            # 假设视频标题中包含某些特定关键字来验证类型（可以根据实际情况修改验证逻辑）
            if category.lower() not in video_title.lower():
                print(f"警告：视频 '{video_title}' 不属于 '{category}' 类别")
            else:
                print(f"视频 '{video_title}' 属于 '{category}' 类别")

    except Exception as e:
        print(f"在验证 '{category}' 类别时出现错误：{e}")

    # 返回首页进行下一个测试
    driver.get("https://tubitv.com/")
    time.sleep(3)

# 关闭浏览器
driver.quit()
