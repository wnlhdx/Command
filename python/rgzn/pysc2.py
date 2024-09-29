import pysc2
from pysc2 import lib

# 初始化游戏环境
with pysc2.env.sc2_env.SC2Env(
        map_name="MoveToBeacon",
        players=[
            lib.features.Player(race=lib.constants.RACE_TERRAN),
        ]) as env:
    action = {
        "action": lib.actions.FUNCTIONS.no_op.Function(),
        "observation": env.reset(),
    }

    while not action["observation"]["step_type"] == pysc2.lib.actions.StepType.LAST:
        # 获取观察结果
        observation = action["observation"]
        player_relative = observation["screen"][0]

        # 寻找兵营
        if observation["control"][0] == 0:
            target = lib.features.Point(x=16, y=16)
            action["action"] = lib.actions.FUNCTIONS.SelectIdleWorker("select_all", "select_all_type")
            action["action"] = lib.actions.FUNCTIONS.BuildQueue("queue_quick", 0)
            action["action"] = lib.actions.FUNCTIONS.Build_Minerals("now", target)
        else:
            action["action"] = lib.actions.FUNCTIONS.no_op("")

        # 执行动作并获取反馈
        action = env.step(action["action"])

    # 游戏结束
    print("Minerals mined:", env.sc2_env.score_cumulative[0])