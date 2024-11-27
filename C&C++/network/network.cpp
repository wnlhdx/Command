#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

// 定义一个简单的分组结构
struct Packet {
    int id;             // 分组ID
    int size;           // 分组大小（比特）
    int destination;    // 分组目的地
};

// 定义链路结构
struct Link {
    int bandwidth;      // 链路带宽（比特/秒）
    int bufferSize;     // 输出缓冲区大小（分组数量）
    std::queue<Packet> buffer; // 输出缓冲区
};

// 定义路由表条目结构
struct RouteEntry {
    int destination; // 目的地地址
    int linkIndex;   // 到达目的地的链路索引
};

// 定义路由器结构
struct Router {
    std::vector<Link> links; // 路由器连接的链路
    std::unordered_map<int, RouteEntry> routingTable; // 路由表
};

// 初始化网络
void initializeNetwork(Router& router, int numLinks, int bandwidth, int bufferSize) {
    router.links.resize(numLinks);
    for (auto& link : router.links) {
        link.bandwidth = bandwidth;
        link.bufferSize = bufferSize;
    }
}

// 发送分组到路由器
void sendPacket(Router& router, Packet packet) {
    // 查找路由表，确定到达目的地的链路索引
    auto routeIt = router.routingTable.find(packet.destination);
    if (routeIt == router.routingTable.end()) {
        std::cerr << "Destination not found in routing table for packet " << packet.id << ".\n";
        return;
    }

    int linkIndex = routeIt->second.linkIndex;
    Link& link = router.links[linkIndex];

    // 模拟输出缓冲区
    if (link.buffer.size() < link.bufferSize) {
        link.buffer.push(packet);
        std::cout << "Packet " << packet.id << " added to link " << linkIndex
            << " buffer. Size: " << packet.size << " bits.\n";
    }
    else {
        // 缓冲区满，分组丢失
        std::cerr << "Packet " << packet.id << " dropped due to full buffer on link " << linkIndex << ".\n";
    }
}

// 处理链路上的分组
void processPackets(Router& router) {
    for (size_t i = 0; i < router.links.size(); ++i) {
        Link& link = router.links[i];
        if (!link.buffer.empty()) {
            Packet packet = link.buffer.front();
            link.buffer.pop();

            // 模拟存储转发延迟
            int delayMs = packet.size * 1000 / link.bandwidth;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));

            std::cout << "Packet " << packet.id << " processed on link " << i
                << " and forwarded. Delay: " << delayMs << " ms.\n";
        }
    }
}

// 更新路由表
void updateRoutingTable(Router& router) {
    router.routingTable[1] = { 1, 0 }; // 到达目的地1使用链路0
    router.routingTable[2] = { 2, 1 }; // 到达目的地2使用链路1
}

// 模拟运行
void runSimulation() {
    Router router;
    initializeNetwork(router, 2, 1000000, 2); // 2个链路，每个1Mbps，缓冲区大小为2

    // 更新路由表
    updateRoutingTable(router);

    // 模拟发送分组
    sendPacket(router, { 1, 500, 1 }); // 分组ID为1，大小500比特，目的地1
    sendPacket(router, { 2, 2000, 2 }); // 分组ID为2，大小2000比特，目的地2
    sendPacket(router, { 3, 1500, 1 }); // 分组ID为3，大小1500比特，目的地1
    sendPacket(router, { 4, 1000, 1 }); // 分组ID为4，大小1000比特，目的地1（会丢弃）

    // 处理链路上的分组
    processPackets(router);
    processPackets(router); // 再次处理以清空缓冲区
}

int main() {
    runSimulation();
    return 0;
}
