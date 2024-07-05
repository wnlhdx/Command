
# 定义集合
A = {1, 2, 3}
B = {2, 3, 4}

# 集合运算
union = A | B       # 并集 {1, 2, 3, 4}
intersection = A & B # 交集 {2, 3}
difference = A - B  # 差集 {1}

# 定义一个简单的函数
def f(x):
    return x ** 2

# 使用 lambda 表达式定义
g = lambda x: x + 1

# 使用函数
print(f(2))  # 输出: 4
print(g(2))  # 输出: 3

# 定义一个关系 (例如：大于关系)
relation = {(1, 2), (2, 3), (3, 4)}

# 检查一个对是否在关系中
print((2, 3) in relation)  # 输出: True

# 定义加法作为一个二元运算
def add(x, y):
    return x + y

# 使用 add 函数
print(add(2, 3))  # 输出: 5


from sympy import symbols, Eq

# 定义整数变量
a, b, c = symbols('a b c')

# 1. 自反性: a = a
reflexive = Eq(a, a)

# 打印和验证自反性
print(f"自反性: {reflexive}  -> {reflexive == True}")

# 2. 对称性: a = b implies b = a
# 创建等式 a = b
eq1 = Eq(a, b)
# 创建等式 b = a
eq2 = Eq(b, a)

# 打印和验证对称性
print(f"对称性: {eq1}  -> {eq2}  -> {eq1 == eq2}")

# 3. 传递性: a = b and b = c implies a = c
eq3 = Eq(b, c)
# 创建等式 a = c
eq4 = Eq(a, c)

# 打印和验证传递性
# 由于 SymPy 中，eq1 和 eq3 的同时成立隐含 eq4 的成立
transitive = Eq(eq1.lhs, eq3.rhs)

print(f"传递性: {eq1} 和 {eq3}  -> {eq4}  -> {transitive == eq4}")

# 在传递性验证中，我们可以直接检查 eq1, eq3 和 eq4 是否都成立
transitive_check = eq1 & eq3 & eq4
print(f"传递性验证: {transitive_check}")

class MathObj:
    A={}
    def Compare(self,obj:"MathObj"):
        return self
    def Function(self):
        return self

    def Binary_Opeation(self,obj:"MathObj"):
        return self



