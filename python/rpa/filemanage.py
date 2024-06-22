import os
import subprocess

def list_files_termux(directory):
    # 使用ADB在Termux上列出指定目录下的文件
    files = subprocess.run(["adb", "shell", "ls", "-l", directory], capture_output=True).stdout.decode('utf-8').split("\n")
    return files[1:]  # 去除头部信息

def classify_files_termux(files):
    # 根据文件扩展名分类文件
    categories = {}
    for file in files:
        extension = file.split(".")[-1]
        if extension in categories:
            categories[extension].append(file)
        else:
            categories[extension] = [file]
    return categories

def read_unix():
    directory = "YOUR_DIRECTORY_PATH"
    files = list_files_termux(directory)
    categories = classify_files_termux(files)
    print(categories)

def list_files_windows(directory):
    # 在Windows上列出指定目录下的文件
    return os.listdir(directory)

def classify_files_windows(files):
    # 根据文件扩展名分类文件
    categories = {}
    for file in files:
        extension = file.split(".")[-1]
        if extension in categories:
            categories[extension].append(file)
        else:
            categories[extension] = [file]
    return categories

# 使用示例
def read_windows()
    directory = "YOUR_DIRECTORY_PATH"
    files = list_files_windows(directory)
    categories = classify_files_windows(files)
    print(categories)

# 主函数
def read():
    # 读取软件信息
    if os.name == 'posix':
        # Unix-like 系统，例如 Termux
        read_unix()
    elif os.name == 'nt':
        # Windows 系统
        read_windows()


if __name__ == '__main__':
    read()
