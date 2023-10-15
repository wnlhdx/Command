from nes_py.wrappers import JoypadSpace
import gym_zelda_1
from gym_zelda_1.actions import MOVEMENT
import time
from matplotlib import pyplot as plt

env = gym_zelda_1.make('Zelda1-v0')
env = JoypadSpace(env, MOVEMENT)

done = True
for step in range(5000):
    if done:
        state = env.reset()
        print(state.shape)
    state, reward, done, info = env.step(env.action_space.sample())
    time.sleep(0.01)
    env.render()
env.close()

