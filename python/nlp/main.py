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
nltk.download('wordnet')
  
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

lemmas = tokens.lemmas()  
print("NLTK形态学分析结果：",lemmas)


# 使用spaCy进行语义学和语用学分析
doc = nlp(sentence)  
for chunk in doc.chunked():  
    print(chunk.text, chunk.dep_, chunk.head.text)
    if chunk.label_ == 'dog':  
        print(chunk.text, chunk.label_)  
        break

