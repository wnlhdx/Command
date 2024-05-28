import matplotlib.pyplot as plt
import numpy as np

# 定义生物圈的大小
world_size = (100, 100)

# 初始化生物圈
world = np.zeros(world_size, dtype=int)

# 定义生物种类和初始位置
species = ['grass', 'herbivore', 'carnivore']
species_positions = {
    'grass': [(10, 10), (20, 30), (30, 40)],
    'herbivore': [(50, 50), (60, 60), (70, 70)],
    'carnivore': [(80, 80), (90, 90), (10, 90)]
}

# 定义生物的移动和繁殖规则
def move_species(species_name, positions):
    # 随机选择一个生物并移动
    species_pos = np.random.choice(positions)
    new_pos = (species_pos[0] + np.random.randint(-1, 2), species_pos[1] + np.random.randint(-1, 2))
    # 检查新位置是否在生物圈内
    if 0 <= new_pos[0] < world_size[0] and 0 <= new_pos[1] < world_size[1]:
        # 更新位置
        positions[species_pos] = new_pos
        # 随机选择一个新位置并添加生物
        if np.random.random() < 0.05:  # 假设繁殖概率为5%
            new_species_pos = (np.random.randint(0, world_size[0]), np.random.randint(0, world_size[1]))
            if new_species_pos not in positions.values():
                positions[new_species_pos] = new_species_pos

# 模拟生物圈
for step in range(100):
    # 移动每个物种
    for species_name in species:
        move_species(species_name, species_positions[species_name])

    # 绘制当前状态
    plt.imshow(world, cmap='viridis')
    plt.scatter(*zip(*species_positions['grass']), color='green', marker='o', s=100)
    plt.scatter(*zip(*species_positions['herbivore']), color='blue', marker='o', s=100)
    plt.scatter(*zip(*species_positions['carnivore']), color='red', marker='o', s=100)
    plt.title(f'生物圈模拟 - 步骤 {step}')
    plt.show()

# 输出生物的位置
print(species_positions)
