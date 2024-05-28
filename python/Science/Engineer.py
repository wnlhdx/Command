from fenics import *

# 创建一个简单的正方体 mesh
mesh = UnitCubeMesh(8, 8, 8)

# 定义函数空间
V = VectorFunctionSpace(mesh, 'P', 1)

# 边界条件
def boundary(x, on_boundary):
    return on_boundary

bc = DirichletBC(V, Constant((0, 0, 0)), boundary)

# 材料参数（弹性模量和泊松比）
E = 70e9
nu = 0.3
mu = E / (2 * (1 + nu))
lmbda = E * nu / ((1 + nu) * (1 - 2 * nu))

# 应力-应变关系（线性弹性模型）
def sigma(v):
    return 2.0 * mu * epsilon(v) + lmbda * tr(epsilon(v)) * Identity(len(v))

# 应变计算
def epsilon(v):
    return sym(nabla_grad(v))

# 载荷
f = Constant((0, 0, -1e3))

# 设定弱形式
u = TrialFunction(V)
d = u.geometric_dimension()  # 维度
v = TestFunction(V)
a = inner(sigma(u), nabla_grad(v)) * dx
L = dot(f, v) * dx

# 求解
u = Function(V)
solve(a == L, u, bc)

# 输出结果
file = File("displacement.pvd")
file << u

# 可以使用ParaView等工具查看结果