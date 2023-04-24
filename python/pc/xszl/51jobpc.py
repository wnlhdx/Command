import openpyxl
import bs4
import requests as r
from selenium import webdriver

h = {"User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:88.0) Gecko/20100101 Firefox/88.0"}  # 伪造浏览器信息，第二步会用到。


def get_joburllist():  # 定义函数get_joburllist

    urllists = []  # 一个列表变量，用于接收所有网页详情页链接
    chrome_driver = r'C:\Users\Administrator\AppData\Local\Programs\Python\Python36\chromedriver.exe'  # chromedriver的文件位置，r代表原始路径
    wd = webdriver.Chrome(executable_path=chrome_driver)  # 创建浏览器，这里用谷歌浏览器，后接驱动器地址
    wd.implicitly_wait(2)  # 智能等待一个元素被发现，或一个命令完成。如果超出了设置时间的则抛出异常。
    # 访问51job网站
    for i in range(1, 34):  # 这里两个参数，（1，34），1代表当前页，34代表总页数，可自行修改
        wd.get(
            "https://search.51job.com/list/010000,000000,0000,00,9,99,%E7%BD%91%E7%BB%9C%E5%AE%89%E5%85%A8,2,i.html")  # 访问链接内的地址，i为第i页

        joblists = wd.find_elements_by_xpath(
            '//div[@class="j_joblist"]/div/a')  # 查找所有div下class属性为"j_joblist"的子节点为div的子节点为a的标签
        for urllist in joblists:  # 循环获取所有详情页的href链接
            href_list = urllist.get_attribute("href")  # 获取href属性的内容
            urllists.append(href_list)
    wd.quit()  # 关闭浏览器
    # print(urllists)
    return urllists


def get_html(url):  # 这里的url指的是第一步获取的详情页的url
    response = r.get(url, headers=h)  # 伪造请求头信息
    response.encoding = "gbk"  # 更改编码为gbk
    return response.text  # 返回html源码


def get_textinfo(html):  # 进行源码筛选
    jobname = []  # 以下为定义模块信息 ， 工作名称
    jobmoney = []  # 工资
    comname = []  # 公司名称
    jobjy = []  # 工作经验
    jobyh = []  # 岗位福利
    jobneed = []  # 岗位职责
    cominfo = []  # 公司信息

    soup = bs4.BeautifulSoup(html, "html.parser")  # 两个参数，第一个参数为html源码，第二个参数为解析html文件，返回结果给soup
    # Beautiful Soup将复杂HTML文档转换成一个复杂的树形结构,每个节点都是Python对象

    # 这里要做一下说明，因为获取到的详情页中，有一部分并没有按照规定模板来进行显示，
    # 而是直接跳转到公司官网，所以进行筛选时会报错，这里用if来做一个简单判断，
    # 如果获取的html源码中并不是按照规定模板来显示的，我们将pass掉这个信息

    jn = soup.find("div", class_="cn")
    strjn = str(jn)  # 将类型转换为str，方便查找
    jnfind = strjn.find("h1")  # 查找“h1”字符，并将查找结果给jhfind
    if jnfind == -1:  # 如果返回结果为 -1 代表没有h1标签 （find没有找到时会返回-1）
        # print ("no")
        joblist = ["查看此招聘信息请点击链接："]  # 则返回空表

    else:  # 否则代表该源码符合规定模板，正常进行筛选
        # print(jn.h1.text)
        jobname.append(jn.h1.text)  # 添加h1标签文本信息

        jm = soup.find("div", class_="cn")
        jobmoney.append(jm.strong.text)  # 添加strong标签文本信息

        cn = soup.find("a", class_="catn")
        comname.append(cn.text)  # 添加公司名称

        jj = soup.find("p", class_="msg ltype")
        jobjy.append(jj.text.replace("\xa0\xa0|\xa0\xa0", " "))  # 添加工作经验文本信息，并将\xa0替换成空字符串，方便查看

        jy = soup.findAll("span", class_="sp4")  # 福利信息这里有点特殊，每个福利都是独立的，我们需要将他放在一个字符串里，并添加到福利信息列表。
        jystr = ""  # 建立空字符串
        for i in jy:  # 从jy（jy代表找到的所有符合条件的标签）中逐个调用并添加到jystr中，以空格隔开。
            jystr += i.text + " "
        jobyh.append(jystr)  # 将完成的字符串添加到列表中

        jn = soup.find("div", class_="bmsg job_msg inbox")
        jnstr = ""
        for i in jn.findAll("p"):  # 跟上一个同理，findAll所有'p'标签 并放到一个空字符串中
            jnstr += i.text + " "
        jobneed.append(jnstr)

        ci = soup.find("div", class_="tmsg inbox")
        cominfo.append(ci.text)  # 添加公司信息

        joblist = jobname + jobmoney + comname + jobjy + jobyh + jobneed + cominfo  # 将独立的列表信息合并到一起，成为一个大的列表。
    return joblist


def save_info():
    wb = openpyxl.Workbook()  # 获取工作簿 就是一个excle层面的对象，在获取工作簿的同时已经创建了一个默认的工作表
    ws = wb.active  # 获取当前工作表
    titlelist = ["岗位名称", "岗位薪资", "公司名称", "工作经验", "岗位诱惑", "岗位职责", "公司信息",
                 "岗位网址"]  # 先创建一个title，方便我们Excel表的查看。
    ws.append(titlelist)  # 添加titlelist到Excel表中
    joburllist = get_joburllist()  # 先调用第一步，获得所有详情页的url列表，并保存到"joburllist"中
    for i in joburllist:
        html = get_html(i)  # 依次取出每一个详情页的url，并获取这个岗位的源码
        jobinfolist = get_textinfo(html)  # 对每个岗位扣取文字信息,并返回一个列表
        jobinfolist.append(i)  # 添加岗位网址信息
        ws.append(jobinfolist)  # 将扣取得文字信息，写入到excel的内存空间
    wb.save("51jobplusmax.xlsx")  # 保存


if __name__ == "__main__":  # 直接执行(F5)
    save_info()  # 执行save_info() 函数

    # 这些是测试信息
    # urllists = get_joburllist()
    # html = get_html(urllists)
    # get_textinfo(html)
    # print(urllists)
