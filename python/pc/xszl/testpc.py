# basic_test.py
import pytest
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

class TestBasic:
    def setup_method(self, method):
        self.driver = webdriver.Firefox()
        self.driver.get("http://www.google.com")

    def teardown_method(self, method):
        self.driver.quit()

    def google_example_that_searches_for(self, search_string):
        search_field = self.driver.find_element(By.NAME, "q")
        search_field.clear()
        search_field.send_keys(search_string)
        print("Page title is: " + self.driver.title)
        search_field.submit()
        WebDriverWait(self.driver, 10).until(
            EC.title_contains(search_string.lower())
        )
        print("Page title is: " + self.driver.title)

    @pytest.mark.parametrize("search_string", ["Cheese!", "Milk!"])
    def test_google_search(self, search_string):
        self.google_example_that_searches_for(search_string)

# 如果你想要分别运行两个测试，可以这样做：
# def test_google_cheese_example(self):
#     self.google_example_that_searches_for("Cheese!")

# def test_google_milk_example(self):
#     self.google_example_that_searches_for("Milk!")
