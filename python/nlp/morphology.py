import argparse
import pandas as pd
import spacy
from tqdm import tqdm
from pathlib import Path

# 初始化NLP模型
nlp = spacy.load("en_core_web_sm")


def load_data(excel_path):
    """读取三张表格的数据"""
    with pd.ExcelFile(excel_path) as xls:
        # # 第一张表C列（带标题）
        # df1 = pd.read_excel(xls, sheet_name=0, usecols="C", header=0)
        # vocab1 = df1.iloc[:, 0].dropna().astype(str).str.strip().tolist()

        # 第二张表A列（无标题）
        df2 = pd.read_excel(xls, sheet_name=1, usecols="A", header=None)
        vocab2 = df2.iloc[:, 0].dropna().astype(str).str.strip().tolist()

        # 第三张表A1（进度存储）
        try:
            df3 = pd.read_excel(xls, sheet_name=2, usecols="A", header=None, nrows=1)
            counter = int(df3.iloc[0, 0]) if not df3.empty else 0
        except:
            counter = 0

    # 合并去重词汇
    # full_vocab = list(set(vocab1 + vocab2))
    return vocab2, counter


def save_progress(excel_path, counter):
    """更新进度到第三张表"""
    df = pd.DataFrame([counter])
    with pd.ExcelWriter(excel_path, engine='openpyxl', mode='a', if_sheet_exists='replace') as writer:
        df.to_excel(writer, sheet_name='Sheet3', index=False, header=False)


def enhanced_analysis(word):
    """增强形态学分析"""
    doc = nlp(word)
    if len(doc) == 0:
        return None

    token = doc[0]
    return {
        '单词': token.text,
        '词根': token.lemma_,
        '词性': token.pos_,
        '前缀': token.prefix_,
        '后缀': token.suffix_,
        '形态特征': dict(token.morph),
        '词族': [child.text for child in token.children if child.dep_ == 'compound'],
        '中文翻译': get_translation(token.text),  # 需要实现翻译查询函数
        '例句': generate_example(token.text)  # 需要实现例句生成函数
    }


def print_analysis(results):
    """增强版命令行输出"""
    for item in results:
        print(f"\n\033[1;36m=== {item['单词']} ===\033[0m")
        print(f"\033[1;32m词根\033[0m: {item['词根']}")
        print(f"\033[1;32m词性\033[0m: {item['词性']}")
        print(f"\033[1;32m词缀\033[0m: 前『{item['前缀']}』+ 后『{item['后缀']}』")
        print(f"\033[1;32m形态特征\033[0m: {item['形态特征']}")
        print(f"\033[1;33m词族\033[0m: {', '.join(item['词族']) or '无'}")
        print(f"\033[1;34m中文\033[0m: {item['中文翻译']}")
        print(f"\033[1;35m例句\033[0m: {item['例句']}")
        print("-" * 80)


def main(excel_path, batch_size=30, start_batch=None):
    # 加载数据
    all_vocab, saved_counter = load_data(excel_path)
    total = len(all_vocab)

    # 参数处理逻辑
    if start_batch is not None:
        start_index = start_batch * batch_size
        end_index = start_index + batch_size
        current_batch = all_vocab[start_index:end_index]
        results = [enhanced_analysis(w) for w in current_batch]
        print_analysis([r for r in results if r])
        return

    # 正常执行模式
    try:
        for i in tqdm(range(saved_counter, min(saved_counter + 2000, total // batch_size))):
            start = i * batch_size
            end = start + batch_size
            current_batch = all_vocab[start:end]

            # 执行分析
            results = [enhanced_analysis(w) for w in current_batch]
            print_analysis([r for r in results if r])

            # 更新进度
            save_progress(excel_path, i + 1)

    except KeyboardInterrupt:
        print("\n进度已自动保存")
    except Exception as e:
        print(f"发生错误: {str(e)}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("excel", help="Excel文件路径")
    parser.add_argument("-j", "--jump", type=int, help="跳转到指定批次")
    args = parser.parse_args()

    if not Path(args.excel).exists():
        print("错误：文件不存在")
        exit(1)

    main(args.excel, start_batch=args.jump)


def get_translation(word, requests=

                    ):
    """使用MyMemory翻译API获取中文翻译"""
    try:
        response = requests.get(
            TRANSLATE_API,
            params={'q': word, 'langpair': 'en|zh-CN'},
            timeout=5
        )
        data = response.json()
        return data['responseData']['translatedText'].split(',')[0]
    except:
        return "翻译获取失败"


def generate_example(word):
    """使用DictionaryAPI获取例句"""
    try:
        response = requests.get(f"{EXAMPLE_API}/{word}", timeout=5)
        data = response.json()

        # 优先选择现代例句
        for meaning in data[0].get('meanings', []):
            for definition in meaning.get('definitions', []):
                if 'example' in definition:
                    return definition['example']

        # 备用例句来源
        return "暂无权威例句"
    except:
        return "例句获取失败