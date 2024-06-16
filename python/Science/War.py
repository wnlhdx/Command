import random

class CombatUnit:
    def __init__(self, name, strength, attack_power, defense, morale):
        self.name = name
        self.strength = strength
        self.attack_power = attack_power
        self.defense = defense
        self.morale = morale  # 意志力

    def attack(self, other):
        damage = self.attack_power - other.defense
        damage = max(0, damage)  # 伤害不能小于0
        other.strength -= damage
        print(f"{self.name} attacks {other.name}, causing {damage} damage. {other.name}'s remaining strength: {other.strength}")

    def is_alive(self):
        return self.strength > 0 and self.morale > 0  # 战斗力和士气都需要大于0

def battle_simulation(unit1, unit2):
    while unit1.is_alive() and unit2.is_alive():
        if random.random() < 0.1:  # 引入不可预测因素，如意外情况，10%的概率该回合不发生战斗
            print("An unexpected event occurs. No combat this round.")
        else:
            unit1.attack(unit2)
            if unit2.is_alive():
                unit2.attack(unit1)
            # 每轮战斗后，双方的士气可能会受到影响
            unit1.morale *= 0.99
            unit2.morale *= 0.99

    winner = unit1 if unit1.is_alive() else unit2
    print(f"{winner.name} wins!")

# 初始化战斗单位，增加防御力和士气属性
unit1 = CombatUnit("Unit A", 100, 15, 5, 1.0)
unit2 = CombatUnit("Unit B", 120, 10, 4, 1.0)

# 进行战斗模拟
battle_simulation(unit1, unit2)
