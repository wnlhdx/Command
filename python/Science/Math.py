
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


# 实数系统：浮动小数
a = 3.14  # 实数

# 整数系统
b = 5  # 整数

# 复数
c = 2 + 3j  # 复数

print(f"Real number: {a}")
print(f"Integer: {b}")
print(f"Complex number: {c}")

# 定义群（以加法群为例）
# 闭合性、结合性、单位元素、逆元素
G = set(range(-10, 11))  # 整数集

def group_add(a, b):
    return a + b

# 检查单位元素（0）和逆元素（相反数）
a = 5
inverse = -a  # 逆元素

print(f"Group G: {G}")
print(f"Group addition: {group_add(a, inverse)} = 0 (identity element)")

# 定义环（以整数加法和乘法为例）
def ring_add(a, b):
    return a + b

def ring_multiply(a, b):
    return a * b

# 检查加法和乘法的分配律
x, y, z = 2, 3, 4
distributive_property = ring_multiply(x, ring_add(y, z)) == ring_add(ring_multiply(x, y), ring_multiply(x, z))

print(f"Ring addition: {ring_add(x, y)}")
print(f"Ring multiplication: {ring_multiply(x, y)}")
print(f"Distributive property: {distributive_property}")

# 定义域（以有理数为例）
from fractions import Fraction

# 创建有理数集合
a = Fraction(3, 4)  # 3/4
b = Fraction(5, 6)  # 5/6

# 加法和乘法
add_result = a + b
mul_result = a * b
div_result = a / b  # 除法（除以非零元素）

print(f"Field addition: {add_result}")
print(f"Field multiplication: {mul_result}")
print(f"Field division: {div_result}")

import numpy as np

# 定义向量空间（二维向量空间）
v1 = np.array([1, 2])
v2 = np.array([3, 4])

# 向量加法
vector_add = v1 + v2

# 向量数乘
scalar_multiply = 2 * v1

print(f"Vector v1: {v1}")
print(f"Vector v2: {v2}")
print(f"Vector addition: {vector_add}")
print(f"Scalar multiplication: {scalar_multiply}")

