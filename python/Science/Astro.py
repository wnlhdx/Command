def is_leap_year(year):
    """
    判断是否闰年（格里历规则）
    规则：
    - 能被4整除且不能被100整除的是闰年
    - 能被400整除的是闰年
    """
    return (year % 4 == 0 and year % 100 != 0) or (year % 400 == 0)

def day_of_year(year, month, day):
    """
    计算格里历日期是该年的第几天
    """
    # 每月天数
    month_days = [31, 28 + is_leap_year(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
    return sum(month_days[:month-1]) + day

def gregorian_to_julian_day(year, month, day):
    """
    计算格里历日期对应的儒略日数（JD），整数部分
    这里使用经典算法（Fliegel-Van Flandern算法）
    """
    if month <= 2:
        year -= 1
        month += 12
    A = year // 100
    B = 2 - A + (A // 4)
    JD = int(365.25*(year + 4716)) + int(30.6001*(month + 1)) + day + B - 1524
    return JD

def example():
    y, m, d = 2024, 6, 7
    print(f"判断年份 {y} 是否闰年：{is_leap_year(y)}")
    doy = day_of_year(y, m, d)
    print(f"{y}-{m}-{d} 是该年的第 {doy} 天")

    jd = gregorian_to_julian_day(y, m, d)
    print(f"格里历日期 {y}-{m}-{d} 对应的儒略日整数部分是：{jd}")

if __name__ == "__main__":
    example()
