// cross_layer_full.cc

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
    uint32_t attackerIndex = nIoT - 1;
    uint32_t gatewayIndex = nIoT;

    NodeContainer nodes;
    nodes.Create(nIoT + 1);

    // WIFI SETUP
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

    // �� CROSS-LAYER VARIABLES
    std::vector<bool> authenticated(nIoT, false);
    std::vector<double> trust(nIoT, 1.0);

    // CLIENTS
    for (uint32_t i = 0; i < nIoT; i++)
    {
        double interval;
        uint32_t maxPackets;

        // ATTACKER BEHAVIOR
        if (i == attackerIndex)
        {
            interval = 0.05;   // flooding
            maxPackets = 400;
        }
        else
        {
            interval = 0.3;
            maxPackets = 180;
        }

        // =========================
        // �� PHASE 1: AUTHENTICATION
        // =========================
        if (i != attackerIndex)
            authenticated[i] = true;
        else
            authenticated[i] = true; // allow attacker to pass auth (realistic)

        if (!authenticated[i])
            continue;

        // =========================
        // �� PHASE 2: IDS
        // =========================
        if (interval < 0.1)
        {
            trust[i] = 0.2; // suspicious node
        }

        // =========================
        // ⚙️ PHASE 3: ADAPTIVE CONTROL
        // =========================
        if (trust[i] < 0.5)
        {
            // isolate malicious node
            continue;
        }

        // =========================
        // �� NORMAL TRAFFIC
        // =========================
        UdpClientHelper client(interfaces.GetAddress(gatewayIndex), port);

        client.SetAttribute("MaxPackets", UintegerValue(maxPackets));
        client.SetAttribute("Interval", TimeValue(Seconds(interval)));
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

    // FINAL METRICS
    double pdr = (totalRx / totalTx) * 100;
    double avgDelay = totalDelay / totalRx;
    double avgJitter = totalJitter / totalRx;
    double throughput = (totalRx * 256 * 8) / (30.0 * 1000);
    double overhead = (totalTx - totalRx) / totalTx * 100;

    std::cout << "\n====== CROSS-LAYER RESULTS ======\n";

    std::cout << "Packets Sent: " << totalTx << std::endl;
    std::cout << "Packets Received: " << totalRx << std::endl;

    std::cout << "PDR: " << pdr << " %" << std::endl;

    std::cout << "End-to-End Delay: " << avgDelay << " sec" << std::endl;

    std::cout << "Jitter: " << avgJitter << " sec" << std::endl;

    std::cout << "Throughput: " << throughput << " Kbps" << std::endl;

    std::cout << "Communication Overhead: " << overhead << " %" << std::endl;

    std::cout << "=================================\n";

    Simulator::Destroy();
}