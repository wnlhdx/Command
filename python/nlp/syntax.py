import nltk  
from nltk.translate.util import align  
from nltk.tokenize import word_tokenize  
from nltk.corpus import wordnet as wn  
  
def analyze_sentence(sentence):  
    # 使用nltk的word_tokenize将句子分解为单词  
    words = word_tokenize(sentence)  
      
    # 初始化一个空列表来存储动词  
    verbs = []  
      
    # 遍历每一个单词，如果单词是动词，则将其添加到verbs列表中  
    for word in words:  
        if nltk.pos_tag([word])[0][1] in ['VB', 'VBD', 'VBG', 'VBN', 'VBP', 'VBZ']:  
            verbs.append(word)  
      
    # 获取英文和中文的句法差异和中文翻译  
    passive_voice = False  
    for verb in verbs:  
        synsets = wn.synsets(verb)  
        for syn in synsets:  
            for l in syn.lemmas():  
                if l.name() == 'passive':  
                    passive_voice = True  
                    break  
                if l.name() == 'active':  
                    passive_voice = False  
                    break  
        if passive_voice:  
            print(f"英文句子中的被动语态可能转换为中文的被动语态。")  
        else:  
            print(f"英文句子中的主动语态可能转换为中文的主动语态。")  
              
    # 翻译句子到中文  
    from googletrans import Translator  
    translator = Translator()  
    translation = translator.translate(sentence, dest='zh-CN').text  
    print(f"句子的中文翻译：{translation}")  
  
if __name__ == "__main__":  
    sentence = input("请输入一个句子：")  # 从命令行读取输入的句子  
    analyze_sentence(sentence)  # 分析句子的时态和语态，给出句子的中文翻译和中英文句法差异
