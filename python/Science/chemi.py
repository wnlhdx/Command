import math

# 玻尔模型：计算氢原子电子能级
def bohr_energy(n):
    E0 = -13.6  # eV
    return E0 / n**2

# 计算能级跃迁能量
def transition_energy(n1, n2):
    return abs(bohr_energy(n2) - bohr_energy(n1))

# 根据能量计算光波长 (nm)
def energy_to_wavelength(energy_eV):
    h = 4.135667696e-15  # eV·s
    c = 2.99792458e8  # m/s
    wavelength_m = (h * c) / energy_eV
    return wavelength_m * 1e9  # nm

n1, n2 = 3, 2
E = transition_energy(n1, n2)
wl = energy_to_wavelength(E)
print(f"电子从{n1}跃迁到{n2}，能量差={E:.2f} eV，波长={wl:.2f} nm")

import numpy as np
import matplotlib.pyplot as plt

# 无限深势阱波函数
def psi(n, x, L):
    return np.sqrt(2/L) * np.sin(n * np.pi * x / L)

L = 1e-9  # 势阱宽度1nm
x = np.linspace(0, L, 1000)

plt.figure(figsize=(8,4))
for n in range(1, 4):
    plt.plot(x * 1e9, psi(n, x, L), label=f"n={n}")

plt.title("无限深势阱波函数")
plt.xlabel("位置 (nm)")
plt.ylabel("波函数 ψ")
plt.legend()
plt.show()

# 不确定性原理估计
def uncertainty(delta_x):
    hbar = 1.0545718e-34  # J·s
    delta_p = hbar / (2 * delta_x)
    return delta_p

dx = 1e-10  # 0.1 nm位置不确定度
dp = uncertainty(dx)
print(f"位置不确定度={dx} m，对应动量不确定度≈{dp:.2e} kg·m/s")

class Atom:
    def __init__(self, symbol, protons, neutrons, electrons):
        self.symbol = symbol
        self.protons = protons
        self.neutrons = neutrons
        self.electrons = electrons

    def atomic_mass(self):
        return self.protons + self.neutrons

    def __repr__(self):
        return f"{self.symbol}: P={self.protons}, N={self.neutrons}, E={self.electrons}, Mass={self.atomic_mass()}"

# 示例同位素
carbon12 = Atom("C", 6, 6, 6)
carbon14 = Atom("C", 6, 8, 6)

print(carbon12)
print(carbon14)
