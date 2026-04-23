// baseline_full.cc

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main()
{
    uint32_t nIoT = 10;
    uint32_t gatewayIndex = nIoT;

    NodeContainer nodes;
    nodes.Create(nIoT + 1);

    // WIFI
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);

    YansWifiPhyHelper phy;
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    // MOBILITY
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // INTERNET
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // SERVER
    uint16_t port = 9;
    UdpServerHelper server(port);
    ApplicationContainer serverApp = server.Install(nodes.Get(gatewayIndex));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(30.0));

    // CLIENTS
    for (uint32_t i = 0; i < nIoT; i++)
    {
        UdpClientHelper client(interfaces.GetAddress(gatewayIndex), port);

        client.SetAttribute("MaxPackets", UintegerValue(180));
        client.SetAttribute("Interval", TimeValue(Seconds(0.3)));
        client.SetAttribute("PacketSize", UintegerValue(256));

        ApplicationContainer app = client.Install(nodes.Get(i));
        app.Start(Seconds(1.0 + i * 0.2));
        app.Stop(Seconds(30.0));
    }

    // FLOW MONITOR
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop(Seconds(30.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    double totalTx = 0;
    double totalRx = 0;
    double totalDelay = 0;
    double totalJitter = 0;

    auto stats = monitor->GetFlowStats();

    for (auto &flow : stats)
    {
        totalTx += flow.second.txPackets;
        totalRx += flow.second.rxPackets;
        totalDelay += flow.second.delaySum.GetSeconds();
        totalJitter += flow.second.jitterSum.GetSeconds();
    }

    // METRICS
    double pdr = (totalRx / totalTx) * 100;
    double avgDelay = totalDelay / totalRx;
    double avgJitter = totalJitter / totalRx;

    double throughput = (totalRx * 256 * 8) / (30.0 * 1000); // Kbps

    double overhead = (totalTx - totalRx) / totalTx * 100;

    std::cout << "\n========== BASELINE RESULTS ==========\n";

    std::cout << "Packets Sent: " << totalTx << std::endl;
    std::cout << "Packets Received: " << totalRx << std::endl;

    std::cout << "PDR: " << pdr << " %" << std::endl;

    std::cout << "End-to-End Delay: " << avgDelay << " sec" << std::endl;

    std::cout << "Jitter: " << avgJitter << " sec" << std::endl;

    std::cout << "Throughput: " << throughput << " Kbps" << std::endl;

    std::cout << "Communication Overhead: " << overhead << " %" << std::endl;

    std::cout << "=====================================\n";

    Simulator::Destroy();
}