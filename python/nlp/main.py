import nltk  
import spacy  
import numpy  
from nltk import word_tokenize, pos_tag, ne_chunk 
from nltk.tree import Tree

  
# 下载Penn Treebank语料库  
nltk.download('treebank')  
nltk.download('maxent_ne_chunker')  
nltk.download('words')  
nltk.download('punkt')  
nltk.download('averaged_perceptron_tagger')  
  
# 加载spaCy语言模型（例如，英文模型'en_core_web_sm'）  
nlp = spacy.load('en_core_web_sm')  
  
# 使用Penn Treebank进行词性标注  
sentence = "The quick brown fox jumps over the lazy dog"  
tokens = word_tokenize(sentence)  
tagged = pos_tag(tokens)  
print("NLTK词性标注结果：", tagged)  
  
# 对句子进行命名实体识别  
entities = ne_chunk(tagged)  
print("NLTK命名实体识别结果：", entities)  


# 使用spaCy进行词性标注和依赖解析  
doc = nlp(sentence)  
for token in doc:  
    print("单词：", token.text, "- 词性：", token.pos_, "- 时态：", token.tag_)
