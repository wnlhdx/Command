#include <iostream>
#include <queue>
#include <vector>
#include <chrono>
#include <thread>

// 定义一个简单的分组结构
struct Packet {
    int id;             // 分组ID
    int size;           // 分组大小（比特）
    // 可以添加其他属性，如时间戳、目的地等
};

// 定义链路结构
struct Link {
    int bandwidth;      // 链路带宽（比特/秒）
    std::queue<Packet> buffer; // 输出缓冲区
};

// 定义路由器结构
struct Router {
    std::vector<Link> links; // 路由器连接的链路
    // 可以添加其他属性，如路由表等
};

void initializeNetwork(Router& router, int numLinks, int bandwidth) {
    router.links.resize(numLinks);
    for (auto& link : router.links) {
        link.bandwidth = bandwidth;
    }
}

void sendPacket(Link& link, Packet packet) {
    // 模拟发送分组到链路缓冲区
    link.buffer.push(packet);
    std::cout << "Packet " << packet.id << " sent to link with size " << packet.size << " bits.\n";
}

void processPackets(Router& router) {
    for (auto& link : router.links) {
        if (!link.buffer.empty()) {
            Packet packet = link.buffer.front();
            link.buffer.pop();
            // 模拟存储转发传输
            std::this_thread::sleep_for(std::chrono::milliseconds(packet.size * 1000 / link.bandwidth));
            std::cout << "Packet " << packet.id << " processed and forwarded.\n";
        }
    }
}

void runSimulation() {
    Router router;
    initializeNetwork(router, 2, 10000000); // 初始化网络，2个链路，每个1Mbps

    // 模拟发送分组
    Packet packet1 = { 1, 1000 }; // 分组ID为1，大小为1000比特
    sendPacket(router.links[0], packet1);

    // 处理链路上的分组
    processPackets(router);
}

int main() {
    runSimulation();
    return 0;
}
