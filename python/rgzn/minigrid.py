import gymnasium as gym
import minigrid

env = gym.make("MiniGrid-BlockedUnlockPickup-v0")
env.reset()
env.render()
