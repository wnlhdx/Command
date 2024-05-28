import random

class Army:
    def __init__(self, size):
        self.size = size
        self.position = (0, 0)
        self.conquered_territories = []

    def move(self, direction):
        if direction == 'n':
            self.position = (self.position[0], self.position[1] - 1)
        elif direction == 's':
            self.position = (self.position[0], self.position[1] + 1)
        elif direction == 'e':
            self.position = (self.position[0] + 1, self.position[1])
        elif direction == 'w':
            self.position = (self.position[0] - 1, self.position[1])

    def attack(self, other_army):
        if self.position == other_army.position:
            # 计算战斗结果
            if random.choice([True, False]):
                # 攻击者获胜
                other_army.size -= self.size
                self.conquered_territories.append(other_army.conquered_territories)
                other_army.conquered_territories = []
            else:
                # 被攻击者获胜
                self.size -= other_army.size
                other_army.conquered_territories.append(self.conquered_territories)
                self.conquered_territories = []

    def display(self):
        print(f"Army at {self.position}, size: {self.size}, conquered territories: {self.conquered_territories}")

# 创建亚历山大大帝的军队
alexander_army = Army(1000)
alexander_army.position = (0, 0)

# 创建其他军队
darius_army = Army(1500)
darius_army.position = (5, 5)

# 模拟亚历山大大帝的征服
while alexander_army.size > 0 and darius_army.size > 0:
    alexander_army.move('e')
    darius_army.move('w')
    if alexander_army.position == darius_army.position:
        alexander_army.attack(darius_army)

# 打印结果
alexander_army.display()
darius_army.display()