#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ETHER_ADDR_LEN 6
#define ETHER_HDR_LEN 14

// 定义以太网帧头部结构体
struct ether_header {
    unsigned char ether_dhost[ETHER_ADDR_LEN]; // 目的MAC地址
    unsigned char ether_shost[ETHER_ADDR_LEN]; // 源MAC地址
    unsigned short ether_type; // 帧类型
};

int main(int argc, char *argv[]) {
    struct ether_header eh;

    // 设置目的MAC地址
    eh.ether_dhost[0] = 0xff;
    eh.ether_dhost[1] = 0xff;
    eh.ether_dhost[2] = 0xff;
    eh.ether_dhost[3] = 0xff;
    eh.ether_dhost[4] = 0xff;
    eh.ether_dhost[5] = 0xff;

    // 设置源MAC地址
    eh.ether_shost[0] = 0x00;
    eh.ether_shost[1] = 0x11;
    eh.ether_shost[2] = 0x22;
    eh.ether_shost[3] = 0x33;
    eh.ether_shost[4] = 0x44;
    eh.ether_shost[5] = 0x55;

    // 设置帧类型
    eh.ether_type = htons(0x0800); // IPv4

    // 打印以太网帧头部信息
    printf("Destination MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eh.ether_dhost[0], eh.ether_dhost[1], eh.ether_dhost[2],
           eh.ether_dhost[3], eh.ether_dhost[4], eh.ether_dhost[5]);
    printf("Source MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eh.ether_shost[0], eh.ether_shost[1], eh.ether_shost[2],
           eh.ether_shost[3], eh.ether_shost[4], eh.ether_shost[5]);
    printf("Ether Type: %04x\n", ntohs(eh.ether_type));

    return 0;
}