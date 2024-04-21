from selenium import webdriver
from selenium.common import TimeoutException
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import os
import time


def get_record_data(driver, sum):
    # 等待页面加载完成
    wait = WebDriverWait(driver, 0)


    # 获取基础URL和人员URL  
    # base_url = 'https://www.op.gg'
    # person_url = base_url + '/leaderboards/tier?region=kr'
    #
    # # 打开浏览器并导航到人员URL
    # driver.get(person_url)
    #

    #
    # # 找到人员链接并获取href属性
    # person_link = wait.until(EC.presence_of_element_located((By.TAG_NAME, "table"))).find_element(By.TAG_NAME, "a")
    # person_href = person_link.get_attribute("href")
    #
    # # 提取人员名称
    # name = person_href.rsplit('/', 1)[1]
    # print(name)

    person_href="https://www.op.gg/summoners/kr/%EB%82%98%EC%9D%98%EB%B3%B8%EC%83%89-KR1"

    # 打开人员页面  
    driver.get(person_href)
    # 找到所有记录按钮并点击它们
    record_buttons = []


    while len(record_buttons) < sum:
        # 等待更多按钮出现并点击它
        try:
            record_buttons = wait.until(EC.presence_of_all_elements_located((By.CLASS_NAME, "btn-record")))
            print("total:", len(record_buttons))
        except TimeoutException:
            print("total:0")
            record_buttons = []  # 重置record_buttons为空列表
        more_button = wait.until(EC.presence_of_element_located((By.XPATH, "//button[@class='more']")))
        more_button.click()
        time.sleep(3)#等待3秒加载页面

    for btn in record_buttons:
        btn.click()
        driver.switch_to.alert.accept()
        # 关闭遮罩层
        close_button = wait.until(EC.presence_of_element_located((By.CLASS_NAME, "close")))
        close_button.click()
        time.sleep(1)#关闭遮罩层

    # 关闭浏览器（可选）  
    driver.quit()


if __name__ == "__main__":
    filename = "test.txt"
    count = 10
    get_record_data(webdriver.Firefox(), count)  # 使用Firefox WebDriver实例调用函数（确保已安装相应的WebDriver）
