import requests
import time
import json
import re

# 缩写列表
ABBR_LIST = ['u', 'idk', 'wanna', 'gonna', 'lol', 'btw', 'omg', 'thx', 'pls', 'cuz', 'ya', 'luv', 'brb']

# 情感词列表（表达情绪）
EMOTION_WORDS = ['sorry', 'love', 'hate', 'happy', 'sad', 'damn', 'lol', 'ugh', 'wow', 'excited', 'angry']

# 主题关键词示例（可扩展）
TOPIC_KEYWORDS = {
    'academic': ['research', 'study', 'professor', 'university', 'exam', 'thesis'],
    'relationship': ['love', 'friend', 'breakup', 'dating', 'marriage'],
    'life': ['family', 'job', 'work', 'vacation', 'daily'],
    'news': ['politics', 'news', 'government', 'election', 'policy']
}

# 参与者关键词（粗略推断）
PARTICIPANT_KEYWORDS = {
    'student': ['homework', 'exam', 'class', 'study', 'professor'],
    'teacher': ['lecture', 'professor', 'grade', 'assignment'],
    'ordinary_user': []  # 默认
}


def count_words(text, word_list):
    text_lower = text.lower()
    count = 0
    for word in word_list:
        count += len(re.findall(r'\b' + re.escape(word) + r'\b', text_lower))
    return count


def detect_topic(text):
    text_lower = text.lower()
    for topic, keywords in TOPIC_KEYWORDS.items():
        for kw in keywords:
            if kw in text_lower:
                return topic
    return 'general'


def detect_participant(text, subreddit):
    text_lower = text.lower()
    # 优先判断subreddit
    if subreddit.lower() == 'askacademia':
        # 判断teacher or student
        for kw in PARTICIPANT_KEYWORDS['teacher']:
            if kw in text_lower:
                return 'teacher'
        for kw in PARTICIPANT_KEYWORDS['student']:
            if kw in text_lower:
                return 'student'
        return 'ordinary_user'
    else:
        return 'ordinary_user'


def detect_function(text):
    text_lower = text.lower()
    if text_lower.endswith('?'):
        return 'question'
    elif any(word in text_lower for word in ['please', 'help', 'can you']):
        return 'request'
    elif count_words(text, EMOTION_WORDS) > 0:
        return 'express_emotion'
    else:
        return 'statement'


def determine_formality(abbr_count):
    return 'informal' if abbr_count > 1 else 'formal'


def determine_social_distance(text):
    text_lower = text.lower()
    # 亲密度：含昵称、缩写、情感词较多判定为亲密
    intimate_markers = ['dear', 'love', 'honey', 'babe', 'lol', 'brb']
    if any(word in text_lower for word in intimate_markers):
        return 'intimate'
    elif count_words(text_lower, ABBR_LIST) > 2:
        return 'intimate'
    else:
        return 'distant'


def determine_social_status(subreddit):
    # 简单示范，高级环境对应学术板块，普通环境对应闲聊
    if subreddit.lower() == 'askacademia':
        return 'high'
    else:
        return 'low'


def determine_referential_vs_expressive(text):
    emo_count = count_words(text, EMOTION_WORDS)
    words = text.split()
    if len(words) == 0:
        return 'neutral'
    emo_ratio = emo_count / len(words)
    return 'expressive' if emo_ratio > 0.05 else 'referential'


def count_abbr(text):
    text_lower = text.lower()
    count = 0
    for abbr in ABBR_LIST:
        count += len(re.findall(r'\b' + re.escape(abbr) + r'\b', text_lower))
    return count


def label_text(text, subreddit):
    abbr_count = count_abbr(text)
    formality = determine_formality(abbr_count)
    social_distance = determine_social_distance(text)
    social_status = determine_social_status(subreddit)
    referential_expressive = determine_referential_vs_expressive(text)
    participant = detect_participant(text, subreddit)
    topic = detect_topic(text)
    function = detect_function(text)

    return {
        'text': text,
        'abbr_count': abbr_count,
        'formality': formality,
        'social_distance': social_distance,
        'social_status': social_status,
        'referential_expressive': referential_expressive,
        'participant': participant,
        'topic': topic,
        'function': function,
        'environment': 'academic' if subreddit.lower() == 'askacademia' else 'casual'
    }


def fetch_posts(subreddit, size=10):
    url = f'https://api.pushshift.io/reddit/search/submission/?subreddit={subreddit}&size={size}&sort=desc&sort_type=created_utc'
    resp = requests.get(url)
    if resp.status_code == 200:
        return resp.json()['data']
    else:
        print(f"Error fetching posts for {subreddit}")
        return []


def fetch_comments(post_id):
    url = f'https://api.pushshift.io/reddit/comment/search/?link_id={post_id}&size=1000'
    resp = requests.get(url)
    if resp.status_code == 200:
        return resp.json()['data']
    else:
        print(f"Error fetching comments for post {post_id}")
        return []


def collect_and_label_data(subreddits, posts_per_subreddit=10):
    data = {}
    for sr in subreddits:
        print(f"Fetching posts from r/{sr}...")
        posts = fetch_posts(sr, size=posts_per_subreddit)
        sr_data = []
        for post in posts:
            post_id = post['id']
            title = post.get('title', '')
            selftext = post.get('selftext', '')
            labeled_post = {
                'post_id': post_id,
                'title': label_text(title, sr),
                'selftext': label_text(selftext, sr),
            }
            comments = fetch_comments(post_id)
            labeled_comments = []
            for c in comments:
                body = c.get('body', '')
                labeled_comments.append(label_text(body, sr))
            labeled_post['comments'] = labeled_comments
            sr_data.append(labeled_post)
            time.sleep(1)
        data[sr] = sr_data
    return data


if __name__ == "__main__":
    import requests

    subreddits = ['AskAcademia', 'casualconversation']
    reddit_data = collect_and_label_data(subreddits, posts_per_subreddit=10)

    with open('reddit_sociolinguistics_labeled.json', 'w', encoding='utf-8') as f:
        json.dump(reddit_data, f, ensure_ascii=False, indent=2)

    print("爬取和自动标注完成，文件名：reddit_sociolinguistics_labeled.json")
