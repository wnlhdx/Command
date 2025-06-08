from google_play_scraper import reviews
from collections import Counter
import jieba  # 中文分词库


def get_app_reviews(app_id, count=100):
    """
    爬取Google Play评论
    app_id: Google Play应用的包名，例如 'com.adobe.figma'
    count: 爬取多少条评论
    """
    all_reviews = []
    next_token = None

    while len(all_reviews) < count:
        result, next_token = reviews(
            app_id,
            lang='en',  # 评论语言
            country='us',  # 国家
            count=min(40, count - len(all_reviews)),
            continuation_token=next_token
        )
        all_reviews.extend(result)
        if not next_token:
            break

    return all_reviews[:count]


def extract_keywords(reviews_text, top_k=20):
    """
    简单的词频统计（中文用jieba分词）
    """
    all_words = []
    for text in reviews_text:
        # 中文用jieba分词；英文用简单空格分词
        words = jieba.lcut(text)  # 中文
        # words = text.lower().split()  # 英文简单示例
        all_words.extend(words)

    # 过滤掉长度小于2的词
    filtered_words = [w for w in all_words if len(w) > 1]

    counter = Counter(filtered_words)
    return counter.most_common(top_k)


if __name__ == "__main__":
    # 以Figma为例（Google Play包名）
    app_id = 'com.figma.figmaprojects'  # 这个包名仅示例，请确认真实包名
    reviews_data = get_app_reviews(app_id, count=100)

    texts = [r['content'] for r in reviews_data]

    keywords = extract_keywords(texts, top_k=20)
    print("高频关键词：")
    for word, freq in keywords:
        print(f"{word}: {freq}")
