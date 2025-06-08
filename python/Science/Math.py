from itertools import product

class Expr:
    def evaluate(self, assignment):
        raise NotImplementedError()
    def vars(self):
        raise NotImplementedError()

class Var(Expr):
    def __init__(self, name):
        self.name = name
    def evaluate(self, assignment):
        return assignment[self.name]
    def vars(self):
        return {self.name}
    def __repr__(self):
        return self.name

class Not(Expr):
    def __init__(self, operand):
        self.operand = operand
    def evaluate(self, assignment):
        return not self.operand.evaluate(assignment)
    def vars(self):
        return self.operand.vars()
    def __repr__(self):
        return f"¬{self.operand}"

class And(Expr):
    def __init__(self, *operands):
        self.operands = operands
    def evaluate(self, assignment):
        return all(op.evaluate(assignment) for op in self.operands)
    def vars(self):
        s = set()
        for op in self.operands:
            s |= op.vars()
        return s
    def __repr__(self):
        return "(" + " ∧ ".join(map(str, self.operands)) + ")"

class Or(Expr):
    def __init__(self, *operands):
        self.operands = operands
    def evaluate(self, assignment):
        return any(op.evaluate(assignment) for op in self.operands)
    def vars(self):
        s = set()
        for op in self.operands:
            s |= op.vars()
        return s
    def __repr__(self):
        return "(" + " ∨ ".join(map(str, self.operands)) + ")"

class Implies(Expr):
    def __init__(self, antecedent, consequent):
        self.antecedent = antecedent
        self.consequent = consequent
    def evaluate(self, assignment):
        return (not self.antecedent.evaluate(assignment)) or self.consequent.evaluate(assignment)
    def vars(self):
        return self.antecedent.vars() | self.consequent.vars()
    def __repr__(self):
        return f"({self.antecedent} → {self.consequent})"

def all_assignments(vars_):
    for values in product([False, True], repeat=len(vars_)):
        yield dict(zip(vars_, values))

def is_valid_argument(premises, conclusion):
    vars_ = set()
    for p in premises:
        vars_ |= p.vars()
    vars_ |= conclusion.vars()

    for assignment in all_assignments(vars_):
        if all(p.evaluate(assignment) for p in premises):
            if not conclusion.evaluate(assignment):
                print(f"Counterexample found: {assignment}")
                return False
    return True

# ========= 题1 ===========
# (a) We’ll have either a reading assignment (R) or homework problems (H), but we won’t have both homework problems and a test (T).
R = Var('R')
H = Var('H')
T = Var('T')
stmt1a = And(
    Or(R, H),
    Not(And(H, T))
)

# (b) You won’t go skiing (S), or you will and there won’t be any snow (N).
S = Var('S')
N = Var('N')
stmt1b = Or(
    Not(S),
    And(S, Not(N))
)

# (c) √7 ± ≤ 2  —— 这个是数学表达式，不属于命题逻辑，不写代码。

# ========= 题2 ===========
# (a) Either John (J) and Bill (B) are both telling the truth, or neither of them is.
J = Var('J')
B = Var('B')
stmt2a = Or(
    And(J, B),
    And(Not(J), Not(B))
)

# (b) I’ll have either fish (F) or chicken (C), but I won’t have both fish and mashed potatoes (M).
F = Var('F')
C = Var('C')
M = Var('M')
stmt2b = And(
    Or(F, C),
    Not(And(F, M))
)

# (c) 3 is a common divisor of 6, 9, and 15 —— 不是命题逻辑命题，跳过。

# ========= 题3 ===========
# (a) Alice (A) and Bob (B) are not both in the room.
A = Var('A')
B = Var('B')
stmt3a = Not(And(A, B))

# (b) Alice and Bob are both not in the room.
stmt3b = And(Not(A), Not(B))

# (c) Either Alice or Bob is not in the room.
stmt3c = Or(Not(A), Not(B))

# (d) Neither Alice nor Bob is in the room.
stmt3d = Not(Or(A, B))

# ========= 题4 ===========
# (a) Either both Ralph (R) and Ed (E) are tall (T), or both of them are handsome (H).
R = Var('R')
E = Var('E')
T = Var('T')
H = Var('H')
stmt4a = Or(
    And(R, E, T),  # 注意，这里的意思更像 (R和E都是tall), 我们简化为 R和E都tall
    And(R, E, H)   # 同理，都是handsome
)
# 其实更准确是：
stmt4a = Or(
    And(T, And(R, E)),  # both Ralph and Ed are tall
    And(H, And(R, E))   # both are handsome
)

# (b) Both Ralph and Ed are either tall or handsome.
stmt4b = And(
    Or(T, H),  # Ralph
    Or(T, H)   # Ed
)
# 但这不能区分 Ralph和Ed，变量名需分开：
RT = Var('RT')
RH = Var('RH')
ET = Var('ET')
EH = Var('EH')
stmt4b = And(
    Or(RT, RH),
    Or(ET, EH)
)

# (c) Both Ralph and Ed are neither tall nor handsome.
stmt4c = And(
    Not(Or(RT, RH)),
    Not(Or(ET, EH))
)

# (d) Neither Ralph nor Ed is both tall and handsome.
stmt4d = Not(
    Or(
        And(RT, RH),
        And(ET, EH)
    )
)

# ========= 题5 ===========
# 判断哪些是well-formed formulas (这题是语法题，不用写代码)

# ========= 题6 ===========
# 用P表示买裤子，S表示买衬衫
P = Var('P')
S = Var('S')
# (a) ¬(P ∧ ¬S)
stmt6a = Not(And(P, Not(S)))
# (b) ¬P ∧ ¬S
stmt6b = And(Not(P), Not(S))
# (c) ¬P ∨ ¬S
stmt6c = Or(Not(P), Not(S))

# ========= 题7 ===========
# S=Steve happy, G=George happy
S = Var('S')
G = Var('G')
stmt7a = And(Or(S, G), Or(Not(S), Not(G)))  # (S ∨ G) ∧ (¬S ∨ ¬G)
stmt7b = Or(S, And(G, Not(S)), Not(G))       # [S ∨ (G ∧ ¬ S)] ∨ ¬G
stmt7c = Or(S, And(G, Or(Not(S), Not(G))))  # S ∨ [G ∧ (¬ S ∨ ¬G)]

# ========= 题8 ===========
# T=taxes up, D=deficit up
T = Var('T')
D = Var('D')
stmt8a = Or(T, D)  # T ∨ D
stmt8b = And(Not(And(T, D)), Not(And(Not(T), Not(D))))  # ¬(T ∧ D) ∧ ¬(¬T ∧ ¬D)
stmt8c = Or(And(T, Not(D)), And(D, Not(T)))  # (T ∧ ¬D) ∨ (D ∧ ¬T)

# ========== 你可以自己用 is_valid_argument 来验证任何组合了 ==========

# 示例：打印1(a)的变量和表达式
print("题1(a)逻辑表达式:", stmt1a)
print("变量:", stmt1a.vars())

# 你可以按需用：
# is_valid_argument([premise1, premise2, ...], conclusion)

# ======= 题目9各小题 =========

# 9(a)
J = Var('J')  # Jane赢数学奖
P = Var('P')  # Pete赢数学奖
C = Var('C')  # Pete赢化学奖
premises_9a = [
    Not(And(J, P)),    # Jane和Pete不会都赢数学奖
    Or(P, C),          # Pete赢数学奖或化学奖
    J                  # Jane赢数学奖
]
conclusion_9a = C      # Pete赢化学奖

# 9(b)
B = Var('B')  # 主菜是牛肉
F = Var('F')  # 主菜是鱼
Pe = Var('Pe') # 蔬菜是豌豆
Co = Var('Co') # 蔬菜是玉米

premises_9b = [
    Or(B, F),                 # 主菜是牛肉或鱼
    Or(Pe, Co),               # 蔬菜是豌豆或玉米
    Not(And(F, Co))           # 不会同时是鱼和玉米
]
conclusion_9b = Not(And(B, Pe)) # 不会同时是牛肉和豌豆

# 9(c)
Jt = Var('Jt')  # John说真话
Bt = Var('Bt')  # Bill说真话
St = Var('St')  # Sam说真话

premises_9c = [
    Or(Jt, Bt),          # John或Bill说真话
    Or(Not(St), Not(Bt)) # Sam或Bill在说谎(即至少一人不说真话)
]
conclusion_9c = Or(Jt, Not(St)) # John说真话 或 Sam说谎

# 9(d)
S = Var('S')  # 销售增长
B = Var('B')  # 老板开心
E = Var('E')  # 费用增加

premises_9d = [
    Or(And(S, B), And(E, Not(B)))  # 要么销售和老板开心，要么费用和老板不开心
]
conclusion_9d = Not(And(S, E))    # 销售和费用不会同时增加

# ========= 验证 ===========
print("9(a) 论证有效?", is_valid_argument(premises_9a, conclusion_9a))
print("9(b) 论证有效?", is_valid_argument(premises_9b, conclusion_9b))
print("9(c) 论证有效?", is_valid_argument(premises_9c, conclusion_9c))
print("9(d) 论证有效?", is_valid_argument(premises_9d, conclusion_9d))
