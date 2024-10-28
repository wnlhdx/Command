import random

class CombatUnit:
    def __init__(self, name, strength, attack_power, defense, morale, political_objective):
        self.name = name
        self.strength = strength
        self.attack_power = attack_power
        self.defense = defense
        self.morale = morale  # 意志力
        self.political_objective = political_objective  # 政治目标

    def attack(self, other):
        if self.morale > 0.5:  # 只有当士气足够时，才会发动攻击
            damage = self.attack_power - other.defense
            damage = max(0, damage)  # 伤害不能小于0
            other.strength -= damage
            print(f"{self.name} attacks {other.name}, causing {damage} damage. {other.name}'s remaining strength: {other.strength}")
        else:
            print(f"{self.name} is too demoralized to attack.")

    def is_alive(self):
        return self.strength > 0 and self.morale > 0  # 战斗力和士气都需要大于0

    def adjust_morale(self, outcome):
        if outcome == 'win':
            self.morale *= 1.01  # 胜利提高士气
        elif outcome == 'loss':
            self.morale *= 0.98  # 失败降低士气

def battle_simulation(unit1, unit2):
    while unit1.is_alive() and unit2.is_alive() and unit1.political_objective and unit2.political_objective:
        if random.random() < 0.1:  # 引入不可预测因素，如意外情况，10%的概率该回合不发生战斗
            print("An unexpected event occurs. No combat this round.")
        else:
            unit1.attack(unit2)
            if not unit2.is_alive():
                unit1.adjust_morale('win')
                unit2.adjust_morale('loss')
                break
            unit2.attack(unit1)
            if not unit1.is_alive():
                unit2.adjust_morale('win')
                unit1.adjust_morale('loss')
                break

            # 每轮战斗后，双方的士气可能会受到影响
            unit1.morale *= 0.99
            unit2.morale *= 0.99

    winner = unit1 if unit1.is_alive() else unit2
    print(f"{winner.name} wins!")

# 初始化战斗单位，增加政治目标属性
unit1 = CombatUnit("Unit A", 100, 15, 5, 1.0, True)
unit2 = CombatUnit("Unit B", 120, 10, 4, 1.0, True)

# 进行战斗模拟
battle_simulation(unit1, unit2)
