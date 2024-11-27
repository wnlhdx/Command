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
};

// 定义链路结构
struct Link {
    int bandwidth;      // 链路带宽（比特/秒）
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

void initializeNetwork(Router& router, int numLinks, int bandwidth) {
    router.links.resize(numLinks);
    for (auto& link : router.links) {
        link.bandwidth = bandwidth;
        link.buffer = std::queue<Packet>(); // 初始化输出缓冲区
    }
}

void sendPacket(Router& router, int destination, Packet packet) {
    // 查找路由表，确定到达目的地的链路索引
    auto routeIt = router.routingTable.find(destination);
    if (routeIt == router.routingTable.end()) {
        std::cerr << "Destination not found in routing table." << std::endl;
        return;
    }

    // 模拟发送分组到路由器的指定链路缓冲区
    router.links[routeIt->second.linkIndex].buffer.push(packet);
    std::cout << "Packet " << packet.id << " sent to link " << routeIt->second.linkIndex << " with size " << packet.size << " bits.\n";
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

void updateRoutingTable(Router& router) {
    // 简单更新路由表，假设只有一个目的地
    router.routingTable[2] = { 2, 0 }; // 目的地2使用链路0
}

void runSimulation() {
    Router router;
    initializeNetwork(router, 2, 10000000); // 初始化网络，2个链路，每个10Mbps

    // 更新路由表
    updateRoutingTable(router);

    // 模拟发送分组
    Packet packet1 = { 1, 1000 }; // 分组ID为1，大小为1000比特
    sendPacket(router, 2, packet1); // 发送到目的地2

    // 处理链路上的分组
    processPackets(router);
}

int main() {
    runSimulation();
    return 0;
}
