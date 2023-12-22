import requests
from bs4 import BeautifulSoup
import networkx as nx
import matplotlib.pyplot as plt

def search_plato_stanford(term):
    base_url = "https://plato.stanford.edu/"
    search_url = f"{base_url}search/searcher.py?query={term}"

    try:
        # 发送 GET 请求获取搜索结果页面
        response = requests.get(search_url)
        if response.status_code == 200:
            # 使用 BeautifulSoup 解析页面
            soup = BeautifulSoup(response.text, 'html.parser')
            
            # 提取相关信息
            results = soup.find_all('li', class_='resultentry')
            
            # 构建学说关联网络图
            G = nx.Graph()
            
            for result in results:
                page_title = result.find('span', class_='title').text.strip()
                G.add_node(page_title)

                # 获取页面链接
                page_url = base_url + result.find('a')['href']
                page_response = requests.get(page_url)
                page_soup = BeautifulSoup(page_response.text, 'html.parser')
                related_links = page_soup.find_all('a', class_='wiki')
                
                for link in related_links:
                    linked_page_title = link.text.strip()
                    G.add_edge(page_title, linked_page_title)
                
            # 计算节点度数（学说热度）并进行排序
            degrees = dict(G.degree())
            sorted_degrees = sorted(degrees.items(), key=lambda x: x[1], reverse=True)

            # 绘制网络图，节点大小根据度数进行映射
            pos = nx.spring_layout(G)  # 使用Spring布局算法进行节点布局
            plt.figure(figsize=(10, 8))
            nx.draw_networkx_nodes(G, pos, nodelist=dict(sorted_degrees).keys(), node_size=[degree * 10 for _, degree in sorted_degrees], node_color='lightblue', alpha=0.8)
            nx.draw_networkx_edges(G, pos, alpha=0.4)
            nx.draw_networkx_labels(G, pos, font_size=12, font_family='sans-serif')
            plt.axis("off")

            # 保存图像为文件
            plt.savefig('philosophy.png')
            plt.show()  # 显示图形

        else:
            print(f"Error: {response.status_code}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    # 从命令行参数读取搜索词汇
    search_term = "Philosophy of history"
    search_plato_stanford(search_term)