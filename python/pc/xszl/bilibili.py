from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException

# 设置Selenium使用的浏览器驱动类型
driver = webdriver.Chrome()  # 这里假设使用Chrome浏览器

# 打开B站热门视频页面
driver.get('https://www.bilibili.com/v/popular/rank/all')

try:
    # 等待页面加载完成，设置一个合理的等待时间
    WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.CLASS_NAME, "video-title"))
    )

    # 获取所有热门视频的标题
    video_titles = driver.find_elements_by_class_name('video-title')
    for title in video_titles:
        print(title.text)

except TimeoutException:
    print("Loading took too much time!")

finally:
    # 关闭浏览器
    driver.quit()
