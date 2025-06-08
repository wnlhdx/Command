import random

class CombatUnit:
    def __init__(self, name, strength, attack_power, defense, morale,
                 political_objective, resources, fatigue,
                 people_support, commander_courage, strategy_goal):
        self.name = name
        self.strength = strength
        self.attack_power = attack_power
        self.defense = defense
        self.morale = morale
        self.political_objective = political_objective
        self.resources = resources
        self.fatigue = fatigue
        self.people_support = people_support  # 人民热忱
        self.commander_courage = commander_courage  # 指挥官的胆略
        self.strategy_goal = strategy_goal  # 战略目标 destroy_army / lower_morale / attrition

    def attack(self, other):
        if self.morale > 0.5 and self.resources > 0 and self.commander_courage > 0.4:
            base_damage = self.attack_power - other.defense
            damage = max(1, int(base_damage * (1 + random.uniform(-0.2, 0.2))))
            other.strength -= damage
            self.resources -= 1
            print(f"{self.name} attacks {other.name}, causing {damage} damage. Resources left: {self.resources}")
        else:
            print(f"{self.name} hesitates or lacks morale/resources to attack.")

    def is_alive(self):
        return self.strength > 0 and self.morale > 0.1 and self.political_objective

    def adjust_morale(self, outcome):
        if outcome == 'win':
            self.morale = min(1.5, self.morale * 1.1)
            self.fatigue = max(0, self.fatigue - 5)
        elif outcome == 'loss':
            self.morale *= 0.9
            self.fatigue += 10

    def check_political_viability(self):
        if self.resources <= 0 or self.people_support < 0.3:
            print(f"{self.name} can no longer pursue its political objectives.")
            self.political_objective = False

    def recover(self):
        recover_amount = int(5 * self.people_support)
        self.resources += recover_amount
        morale_boost = 0.05 * self.people_support
        self.morale = min(1.5, self.morale + morale_boost)
        print(f"{self.name} is resupplied: +{recover_amount} resources, +{morale_boost:.2f} morale")

def check_victory_condition(unit, opponent):
    if unit.strategy_goal == "destroy_army":
        return opponent.strength <= 0
    elif unit.strategy_goal == "lower_morale":
        return opponent.morale < 0.2
    elif unit.strategy_goal == "attrition":
        return opponent.resources <= 0 or opponent.fatigue > 100
    return False

def battle_simulation(unit1, unit2):
    round_counter = 1
    while unit1.is_alive() and unit2.is_alive():
        print(f"\n--- Round {round_counter} ---")

        unit1.check_political_viability()
        unit2.check_political_viability()

        if not unit1.political_objective or not unit2.political_objective:
            break

        if random.random() < 0.1:
            print("Unexpected event delays action this round.")
        else:
            if unit1.fatigue > 60:
                print(f"{unit1.name} is too fatigued and defends.")
                unit1.attack_power *= 0.9
            if unit2.fatigue > 60:
                print(f"{unit2.name} is too fatigued and defends.")
                unit2.attack_power *= 0.9

            unit1.attack(unit2)
            if check_victory_condition(unit1, unit2):
                print(f"{unit1.name} achieves its strategic goal!")
                unit1.adjust_morale('win')
                unit2.adjust_morale('loss')
                break

            unit2.attack(unit1)
            if check_victory_condition(unit2, unit1):
                print(f"{unit2.name} achieves its strategic goal!")
                unit2.adjust_morale('win')
                unit1.adjust_morale('loss')
                break

            # 消耗与恢复
            unit1.fatigue += 7
            unit2.fatigue += 7
            unit1.morale *= 0.98
            unit2.morale *= 0.98

        # 战斗恢复机制
        if random.random() < 0.3:
            unit1.recover()
            unit2.recover()

        round_counter += 1

    # 战果
    if unit1.is_alive() and not unit2.is_alive():
        print(f"\n{unit1.name} wins by elimination.")
    elif unit2.is_alive() and not unit1.is_alive():
        print(f"\n{unit2.name} wins by elimination.")
    elif not unit1.is_alive() and not unit2.is_alive():
        print("\nBoth units collapse. Mutual defeat.")
    else:
        print("\nWar halted due to political exhaustion.")

# 初始化单位（你可自行调整策略目标）
unit1 = CombatUnit(
    name="Republican Guard",
    strength=120,
    attack_power=18,
    defense=7,
    morale=1.0,
    political_objective=True,
    resources=30,
    fatigue=0,
    people_support=0.8,
    commander_courage=0.9,
    strategy_goal="destroy_army"
)

unit2 = CombatUnit(
    name="National Militia",
    strength=110,
    attack_power=14,
    defense=5,
    morale=1.0,
    political_objective=True,
    resources=35,
    fatigue=0,
    people_support=0.7,
    commander_courage=0.6,
    strategy_goal="lower_morale"
)

# 执行模拟
battle_simulation(unit1, unit2)
