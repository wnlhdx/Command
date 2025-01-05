import pandas as pd
import jieba
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity


# 读取爬取的数据
def load_data(file_path):
    df = pd.read_csv(file_path)
    return df['Content']


# 分词
def tokenize(content):
    return [' '.join(jieba.lcut(text)) for text in content]


# 计算文本相似性（余弦相似度）
def calculate_similarity(tokenized_texts):
    vectorizer = TfidfVectorizer()
    tfidf_matrix = vectorizer.fit_transform(tokenized_texts)
    similarity_matrix = cosine_similarity(tfidf_matrix)
    return similarity_matrix


# 输出语义相似的句子
def find_similar_sentences(content, similarity_matrix, threshold=0.5):
    results = []
    for i in range(len(content)):
        for j in range(i + 1, len(content)):
            if similarity_matrix[i, j] > threshold:
                results.append((content[i], content[j], similarity_matrix[i, j]))
    return results


# 主程序
if __name__ == "__main__":
    # 加载数据
    file_path = "weibo_data.csv"  # 爬取的数据文件
    content = load_data(file_path)

    # 分词与预处理
    tokenized_texts = tokenize(content)

    # 计算相似度
    similarity_matrix = calculate_similarity(tokenized_texts)

    # 查找相似句子
    threshold = 0.6  # 设置相似度阈值
    similar_sentences = find_similar_sentences(content, similarity_matrix, threshold)

    # 打印结果
    print("以下句子表达相似含义：")
    for sent1, sent2, score in similar_sentences:
        print(f"- \"{sent1}\" 和 \"{sent2}\" 的相似度为 {score:.2f}")
