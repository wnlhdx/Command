import random

class CombatUnit:
    def __init__(self, name, strength, attack_power, defense, morale, political_objective, resources, fatigue):
        self.name = name
        self.strength = strength
        self.attack_power = attack_power
        self.defense = defense
        self.morale = morale  # 士气
        self.political_objective = political_objective  # 政治目标
        self.resources = resources  # 资源
        self.fatigue = fatigue  # 疲劳

    def attack(self, other):
        if self.morale > 0.5 and self.resources > 0:  # 只有当士气足够且有资源时，才会发动攻击
            damage = self.attack_power - other.defense
            damage = max(0, damage)  # 伤害不能小于0
            other.strength -= damage
            self.resources -= 1  # 每次攻击消耗资源
            print(f"{self.name} attacks {other.name}, causing {damage} damage. {other.name}'s remaining strength: {other.strength}. Resources left: {self.resources}")
        else:
            print(f"{self.name} is too demoralized or lacks resources to attack.")

    def is_alive(self):
        return self.strength > 0 and self.morale > 0 and self.political_objective  # 战斗力和士气都需要大于0

    def adjust_morale(self, outcome):
        if outcome == 'win':
            self.morale *= 1.01  # 胜利提高士气
            self.fatigue = max(0, self.fatigue - 5)  # 胜利减少疲劳
        elif outcome == 'loss':
            self.morale *= 0.98  # 失败降低士气
            self.fatigue += 10  # 失败增加疲劳

    def check_resources(self):
        if self.resources <= 0:
            print(f"{self.name} has run out of resources and is retreating.")
            self.political_objective = False  # 资源耗尽，目标无法实现

    def recover(self):
        self.resources += 5  # 恢复一定的资源
        self.morale *= 1.05  # 恢复士气
        print(f"{self.name} recovers resources and morale.")

def battle_simulation(unit1, unit2):
    while unit1.is_alive() and unit2.is_alive():
        unit1.check_resources()
        unit2.check_resources()

        if random.random() < 0.1:  # 引入不可预测因素，如意外情况，10%的概率该回合不发生战斗
            print("An unexpected event occurs. No combat this round.")
        else:
            if unit1.fatigue > 50:  # 如果疲劳过高，单位可能选择防守或撤退
                print(f"{unit1.name} chooses to defend due to fatigue.")
                unit1.attack_power *= 0.8
            if unit2.fatigue > 50:
                print(f"{unit2.name} chooses to defend due to fatigue.")
                unit2.attack_power *= 0.8

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

            # 每轮战斗后，双方的士气和疲劳都会受到影响
            unit1.morale *= 0.99
            unit2.morale *= 0.99
            unit1.fatigue += 5
            unit2.fatigue += 5

        # 模拟补给恢复
        if random.random() < 0.2:  # 20%的概率进行补给恢复
            unit1.recover()
            unit2.recover()

    winner = unit1 if unit1.is_alive() else unit2
    print(f"{winner.name} wins!")

# 初始化战斗单位，增加政治目标和资源、疲劳属性
unit1 = CombatUnit("Unit A", 100, 15, 5, 1.0, True, 30, 0)
unit2 = CombatUnit("Unit B", 120, 10, 4, 1.0, True, 25, 0)

# 进行战斗模拟
battle_simulation(unit1, unit2)
