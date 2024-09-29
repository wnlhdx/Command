#include "ns3/core-module.h"  
#include "ns3/network-module.h"  
#include "ns3/mobility-module.h"  
#include "ns3/wifi-module.h"  
#include "ns3/internet-module.h"  
  
# using namespace ns3;
#
# int main(int argc, char *argv[])
# {
#   // 创建节点和链路
#   NodeContainer nodes;
#   nodes.Create(2);
#   InternetStackHelper stack;
#   stack.Install(nodes);
#   NetDeviceContainer devices;
#   devices.Create(2, 1); // 创建两个节点，每个节点一个无线网卡
#   devices.SetForwarding(1, true); // 设置节点1的数据包转发
#   MobilityHelper mobility;
#   mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", 0.0, "MinY", 0.0, "DeltaX", 50.0, "DeltaY", 50.0, "ReferenceAltitude", 2.0);
#   mobility.Install(nodes);
#   devices.EnableAscii(); // 开启设备的信息输出
#   devices.Start(Seconds(1.0)); // 启动设备
#   devices.Stop(Seconds(10.0)); // 停止设备
#   return 0;
# }