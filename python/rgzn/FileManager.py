import sqlite3
import os

database_path  = '..\\..\\..\\Others\\database\\sqlite\\base.db'



# 连接到SQLite数据库
# 如果数据库文件不存在，会自动在当前目录创建
conn = sqlite3.connect(database_path)

# 创建一个游标对象，用于执行SQL语句
cursor = conn.cursor()

# table


# 执行一个SQL查询
# 假设数据库中有一个名为'my_table'的表
query = 'SELECT * FROM my_table'
cursor.execute(query)

# 获取查询结果
rows = cursor.fetchall()

# 打印查询结果
for row in rows:
    print(row)

# 关闭游标
cursor.close()

# 关闭数据库连接
conn.close()
