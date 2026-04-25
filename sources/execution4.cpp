// auth_full.cc 

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"
#include <cmath>

using namespace ns3;

int main()
{
    uint32_t nIoT = 10;
    uint32_t attackerIndex = nIoT - 1;
    uint32_t gatewayIndex = nIoT;

    NodeContainer nodes;
    nodes.Create(nIoT + 1);

    // ---------------- WIFI ----------------
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);

    YansWifiPhyHelper phy;
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    // ---------------- MOBILITY ----------------
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // ---------------- INTERNET ----------------
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // ---------------- SERVER ----------------
    uint16_t port = 9;
    UdpServerHelper server(port);
    ApplicationContainer serverApp = server.Install(nodes.Get(gatewayIndex));
    serverApp.Start(Seconds(0.0));
    serverApp.Stop(Seconds(30.0));

    // ---------------- AUTHENTICATION ----------------
    std::vector<bool> authenticated(nIoT, true);
    authenticated[attackerIndex] = false; // �� attacker blocked

    // ---------------- CLIENTS ----------------
    for (uint32_t i = 0; i < nIoT; i++)
    {
        if (!authenticated[i])
            continue; // block attacker

        UdpClientHelper client(interfaces.GetAddress(gatewayIndex), port);

        client.SetAttribute("MaxPackets", UintegerValue(180));
        client.SetAttribute("Interval", TimeValue(Seconds(0.3)));
        client.SetAttribute("PacketSize", UintegerValue(256));

        ApplicationContainer app = client.Install(nodes.Get(i));
        app.Start(Seconds(1.0 + i * 0.2));
        app.Stop(Seconds(30.0));
    }

    // ---------------- FLOW MONITOR ----------------
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // ---------------- NETANIM ----------------
    AnimationInterface anim("auth.xml");
    anim.EnablePacketMetadata(true);

    // center gateway
    anim.SetConstantPosition(nodes.Get(gatewayIndex), 50, 50);

    double radius = 40;

    for (uint32_t i = 0; i < nIoT; i++)
    {
        double angle = 2 * M_PI * i / nIoT;
        double x = 50 + radius * cos(angle);
        double y = 50 + radius * sin(angle);

        anim.SetConstantPosition(nodes.Get(i), x, y);
    }

    // node colors
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        if (i == gatewayIndex)
        {
            anim.UpdateNodeDescription(nodes.Get(i), "Gateway");
            anim.UpdateNodeColor(nodes.Get(i), 0, 255, 0);
            anim.UpdateNodeSize(i, 1.2, 1.2);
        }
        else if (i == attackerIndex)
        {
            anim.UpdateNodeDescription(nodes.Get(i), "Blocked");
            anim.UpdateNodeColor(nodes.Get(i), 255, 0, 0); // RED
            anim.UpdateNodeSize(i, 1.2, 1.2);
        }
        else
        {
            anim.UpdateNodeDescription(nodes.Get(i), "IoT-" + std::to_string(i));
            anim.UpdateNodeColor(nodes.Get(i), 0, 0, 255);
            anim.UpdateNodeSize(i, 0.8, 0.8);
        }
    }

    Simulator::Stop(Seconds(30.0));
    Simulator::Run();

    // ---------------- RESULTS ----------------
    monitor->CheckForLostPackets();

    double totalTx = 0;
    double totalRx = 0;
    double totalDelay = 0;
    double totalJitter = 0;

    for (auto &flow : monitor->GetFlowStats())
    {
        totalTx += flow.second.txPackets;
        totalRx += flow.second.rxPackets;
        totalDelay += flow.second.delaySum.GetSeconds();
        totalJitter += flow.second.jitterSum.GetSeconds();
    }

    double pdr = (totalTx > 0) ? (totalRx / totalTx) * 100 : 0;
    double avgDelay = (totalRx > 0) ? totalDelay / totalRx : 0;
    double avgJitter = (totalRx > 0) ? totalJitter / totalRx : 0;
    double throughput = (totalRx * 256 * 8) / (30.0 * 1000);
    double overhead = (totalTx > 0) ? (totalTx - totalRx) / totalTx * 100 : 0;

    std::cout << "\n========== AUTHENTICATION RESULTS ==========\n";
    std::cout << "Packets Sent: " << totalTx << std::endl;
    std::cout << "Packets Received: " << totalRx << std::endl;
    std::cout << "PDR: " << pdr << " %" << std::endl;
    std::cout << "End-to-End Delay: " << avgDelay << " sec" << std::endl;
    std::cout << "Jitter: " << avgJitter << " sec" << std::endl;
    std::cout << "Throughput: " << throughput << " Kbps" << std::endl;
    std::cout << "Communication Overhead: " << overhead << " %" << std::endl;
    std::cout << "===========================================\n";

    Simulator::Destroy();
    return 0;
}
