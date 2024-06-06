import nltk
from nltk.corpus import wordnet
import  sys

# 下载nltk中使用的一些资源，如WordNet，如果已经下载过，可以注释掉
nltk.download('wordnet')

# 从命令行获取输入的单词和意思的索引
if len(sys.argv) > 2:
    word = sys.argv[1]
    sense_index = int(sys.argv[2]) - 1  # 减1是因为用户从1开始计数，而列表索引从0开始
else:
    print("请在命令行中提供单词和意思的索引。")
    sys.exit(1)

# 获取单词的所有同义词集
synsets = wordnet.synsets(word)

# 检查索引是否有效
if sense_index < 0 or sense_index >= len(synsets):
    print(f"无效的意思索引。单词 '{word}' 有 {len(synsets)} 个意思。")
    sys.exit(1)

# 获取指定索引的同义词集
synset = synsets[sense_index]

# 打印定义、例句和用法
print(f"单词：{word}")
print(f"意思索引：{sense_index + 1}")
print(f"定义：{synset.definition()}")
print(f"例句：{synset.examples()}")

# 获取同义词集的用法
lexname = synset.lexname()
print(f"用法：{lexname}")

# 打印同义词集的其他信息
print(f"同义词集：{synset}")
print(f"同义词集的词性：{synset.pos()}")
print(f"同义词集的成员单词：{synset.lemma_names()}")
