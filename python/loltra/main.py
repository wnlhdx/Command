
from selenium import webdriver
from selenium.webdriver.common.by import By
import heartrate

heartrate.trace(browser=True)

base_url = 'https://www.op.gg'
person_url = base_url + '/leaderboards/tier?region=kr'
driver = webdriver.Firefox()
driver.get(person_url)
person = driver.find_element(By.TAG_NAME, "table").find_element(By.TAG_NAME, "a").get_attribute("href")
print(person)
name = person.rsplit('/')[1]
print(name)
driver.get(person)
rec=driver.find_elements(By.CLASS_NAME,"record").text
print(rec)
# record = BeautifulSoup(getWebStr(purl), 'lxml')
# record = record.select('#content-container')
# out = open('test.txt', 'w', encoding='utf-8')
# out.write(str(record))
# out2 = open('test.html', 'w', encoding='utf-8')
# out2.write(str(record))
# out.close()
# out2.close()
