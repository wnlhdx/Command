from gym_derk.envs import DerkEnv

env = DerkEnv(
  n_arenas=1 # Change to 100 for instance, to run many instances in parallel
)

observation_n = env.reset()

while True:
  # Take random actions each step
  action_n = [env.action_space.sample() for i in range(env.n_agents)]
  observation_n, reward_n, done_n, info_n = env.step(action_n)

  if all(done_n):
    break
env.close()