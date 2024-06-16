from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.chrome.service import Service as ChromeService
from webdriver_manager.chrome import ChromeDriverManager
import os


# 设置Selenium使用的浏览器驱动类型

def download_driver():
    service = ChromeService(executable_path=ChromeDriverManager().install())
    return service
    # 这里假设使用Chrome浏览器


def get_driver_path():
    driver_path = "../../test/"
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        driver_path += "driver-linux/chromedriver"
    elif os.name == 'nt':
        # Windows 系统
        driver_path += "driver-win/chromedriver.exe"
    return driver_path


driver = webdriver.Chrome(service=download_driver())
# 打开B站热门视频页面
driver.get('https://www.bilibili.com/v/popular/rank/all')

try:
    # 等待页面加载完成，设置一个合理的等待时间
    WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.CLASS_NAME, "title"))
    )

    # 获取所有热门视频的标题
    video_titles = driver.find_elements(By.CLASS_NAME, 'title')
    for title in video_titles:
        print(title.text)

except TimeoutException:
    print("Loading took too much time!")

finally:
    # 关闭浏览器
    driver.quit()
