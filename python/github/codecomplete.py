import logging
from seleniumbase import BaseCase

class LoginTest(BaseCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # 手动初始化 logger
        self.logger = logging.getLogger(__name__)  # 使用当前模块的名称
        self.logger.setLevel(logging.DEBUG)  # 设置日志级别为 DEBUG
        # 设置控制台输出日志格式
        handler = logging.StreamHandler()
        formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)

    def test_login_with_exception_handling(self):
        try:
            # 打开测试登录页面
            self.open("https://the-internet.herokuapp.com/login")

            # 检查页面是否加载成功
            self.assert_element("form", timeout=5)

            # 输入用户名和密码
            self.type("#username", "tomsmith")
            self.type("#password", "incorrect_password")  # 故意输入错误密码

            # 点击登录按钮
            self.click("button[type='submit']")

            # 验证登录失败消息
            self.assert_text("Your password is invalid!", "#flash")

        except Exception as e:
            # 捕获异常并记录详细日志
            error_message = f"Exception encountered during login test: {str(e)}"
            self.logger.error(error_message)

            # 打印当前页面的 HTML，帮助调试
            page_source = self.get_page_source()
            self.logger.error(f"Page source at the time of error: \n{page_source[:1000]}...")

            # 进行截图保存，方便后续分析
            self.save_screenshot("login_test_error.png")
            self.logger.error("Screenshot saved as login_test_error.png")

            # 提供异常信息给用户，方便快速识别
            print(f"Error encountered: {error_message}")
            print(f"Screenshot saved at: login_test_error.png")

            # 重新抛出异常以便后续分析
            raise
        finally:
            # 确保浏览器在异常后也能正确关闭
            if hasattr(self, 'driver'):
                self.driver.quit()
