import spacy  
  
nlp = spacy.load('en_core_web_sm')  
  
def analyze_sentence(sentence):  
    doc = nlp(sentence)  
    verbs = [verb.text for verb in doc.verbs()]  
    tense_aspect = {}  
      
    for verb in verbs:  
        if verb.text.endswith('ed'):  
            tense_aspect[verb] = 'Past'  
        elif verb.text.endswith('ing'):  
            tense_aspect[verb] = 'Present'  
        elif 'will' in verb.text:  
            tense_aspect[verb] = 'Future'  
        elif 'be' in verb.text and 'ing' in verb.text:  
            tense_aspect[verb] = 'Passive'  
        else:  
            tense_aspect[verb] = 'Unknown'  
    return tense_aspect  
  
if __name__ == "__main__":  
    sentence = input("请输入一个句子：")  # 从命令行读取输入的句子  
    result = analyze_sentence(sentence)  # 分析句子的时态和语态  
    print(result)  # 输出结果