import QuantLib as ql

# 设置日期
valuation_date = ql.Date(15, 9, 2023)

# 创建一个日历（这里使用的是TARGET，即欧元区TARGET日历）
calendar = ql.TARGET()

# 创建一个期限结构（这里使用的是flat forward rate）
rate = 0.03  # 假设的年化利率
day_count = ql.Actual365Fixed()
flat_forward = ql.FlatForward(valuation_date, rate, day_count)
term_structure = ql.YieldTermStructureHandle(flat_forward)

# 创建一个债券
issue_date = ql.Date(15, 9, 2020)  # 注意这里使用了 ql.Date
maturity_date = ql.Date(15, 9, 2025)  # 注意这里使用了 ql.Date
coupon_rate = 0.05  # 假设的票面利率
coupon_frequency = ql.Annual
face_value = 100.0  # 面值

# 注意，FixedRateBond 构造函数的参数顺序和类型
bond = ql.FixedRateBond(
    2,  # Settlement days
    face_value,  # Face value
    ql.Schedule(
        issue_date,
        maturity_date,
        ql.Period(coupon_frequency),
        calendar,
        ql.Unadjusted,
        ql.Unadjusted,
        ql.DateGeneration.Backward,
        False
    ),
    [coupon_rate],
    day_count
)

# 创建一个定价引擎
bond_engine = ql.DiscountingBondEngine(term_structure)

# 将定价引擎赋给债券
bond.setPricingEngine(bond_engine)

# 计算债券的价格
bond_price = bond.NPV()
print(f"Bond price: {bond_price:.2f}")
