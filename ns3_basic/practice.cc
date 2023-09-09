/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<map>

// Default Network Topology
//
//  s0              r0
//  s1              r1
//   .    x-----y   .
//   .              .
//  sn              rn

using namespace ns3;
using std::vector;
using std::ofstream;
using std::endl;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int packetSent = 0;

std::map<Address, int> recieved_size;

void onOffTxHelper(Ptr<const Packet> packet){
    packetSent++;
}

void sinkRxHelper(Ptr<const Packet> packet, const Address &address){
    recieved_size[address] += packet->GetSize();
}

int
main(int argc, char* argv[])
{
    // parsing parameters
    const int tx_range = 5;
    const int packetSize = 1024;

    int nNodes = 20;
    int nFlows = 10;
    int nPackets = 100;
    int coverageArea = 1*tx_range;
    int graph = -1;
    std::string throughputfilename = "throughput.dat";
    std::string drfilename = "deliveryratio.dat";

    CommandLine cmd(__FILE__);
    cmd.AddValue("nNodes", "Number of wifi nodes/devices", nNodes);
    cmd.AddValue("nFlows", "Number of flows", nFlows);
    cmd.AddValue("nPackets", "Number of packets per second", nPackets);
    cmd.AddValue("coverageArea", "coverage Area", coverageArea);
    cmd.AddValue("graph", "which attribute will be plotted", graph);
    cmd.AddValue("tpfile", "name of throughput file", throughputfilename);
    cmd.AddValue("drfile", "name of delivery ratio file", drfilename);

    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);

    coverageArea = coverageArea * tx_range;
    int nsNodes = nNodes/2; // no of sender
    int nrNodes = nNodes - nNodes/2; // no of reciever
    nFlows = std::max(nsNodes, nFlows); // no of flows min of sender and given
    uint64_t dataRate = (nPackets * packetSize * 8);


    // LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);
    // LogComponentEnable("PacketSink", LOG_LEVEL_INFO);


    std::cout << nNodes << " " << nFlows << " " << nPackets << " " << coverageArea << std::endl;  
    NodeContainer apNodes;
    apNodes.Create(2);

    // bottleneck p2p connection
    PointToPointHelper p2pHelper;
    NetDeviceContainer apDevices;
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("2Mbps"));
    apDevices = p2pHelper.Install(apNodes);

    // s wifi connection
    NodeContainer sNodes, rNodes;
    sNodes.Create(nsNodes);
    rNodes.Create(nrNodes);
    NodeContainer sApNodes = apNodes.Get(0); // x
    NodeContainer rApNodes = apNodes.Get(1); // y

    
    YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default();
    channel1.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(coverageArea));
    YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default();
    channel2.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(coverageArea));

    YansWifiPhyHelper phy1;
    YansWifiPhyHelper phy2;
    phy1.SetChannel(channel1.Create());
    phy2.SetChannel(channel2.Create());

    WifiMacHelper mac1, mac2;
    Ssid ssid1 = Ssid("ns-3-ssid");
    Ssid ssid2 = Ssid("ns-3-ssid");
    mac1.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid1), "ActiveProbing", BooleanValue(false));
    mac2.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid2), "ActiveProbing", BooleanValue(false));

    WifiHelper wifi1;

    NetDeviceContainer staDevices1;
    staDevices1 = wifi1.Install(phy1, mac1, sNodes);

    NetDeviceContainer staDevices2;
    staDevices2 = wifi1.Install(phy2, mac2, rNodes);

    mac1.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid1));
    mac2.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid2));
    
    NetDeviceContainer apDevices1;
    apDevices1 = wifi1.Install(phy1, mac1, sApNodes);

    NetDeviceContainer apDevices2;
    apDevices2 = wifi1.Install(phy2, mac2, rApNodes);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(1.0),
                                  "DeltaY",
                                  DoubleValue(1.0),
                                  "GridWidth",
                                  UintegerValue(10),
                                  "LayoutType",
                                  StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(sApNodes);
    mobility.Install(sNodes);
    mobility.Install(rApNodes);
    mobility.Install(rNodes);

    // Installing stack
    InternetStackHelper stack;
    stack.Install(sNodes);
    stack.Install(apNodes);
    stack.Install(rNodes);

    // Giving IP address
    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer apNodesInterfaces;
    apNodesInterfaces = address.Assign(apDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer sNodesInterfaces, apNodes1Interfaces;
    sNodesInterfaces = address.Assign(staDevices1);
    apNodes1Interfaces = address.Assign(apDevices1);


    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer rNodesInterfaces, apNodes2Interfaces;
    rNodesInterfaces = address.Assign(staDevices2);
    apNodes2Interfaces = address.Assign(apDevices2);

    // reciever
    int flowPerNode = nFlows / nsNodes;
    for(int i = 0; i < nsNodes; i++){
        for(int j = 0; j <= flowPerNode; j++){
            PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress(rNodesInterfaces.GetAddress(i), 9+j));
            ApplicationContainer sinkApp = sinkHelper.Install(rNodes.Get(i));
            sinkApp.Start(Seconds(0.0));
            sinkApp.Stop(Seconds(10.0));

            Ptr<PacketSink> sink = StaticCast<PacketSink>(sinkApp.Get(0));
            sink->TraceConnectWithoutContext("Rx", MakeCallback(
                &sinkRxHelper
            ));
        }
    }

    // sender
    for(int i = 0; i < nsNodes; i++){
        for(int j = 0; j < flowPerNode; j++){
            OnOffHelper onOffHelper("ns3::TcpSocketFactory", InetSocketAddress(rNodesInterfaces.GetAddress(i), 9+j));
            
            onOffHelper.SetAttribute("PacketSize", UintegerValue(1024));
            onOffHelper.SetAttribute("DataRate", DataRateValue(DataRate(dataRate)));

            ApplicationContainer sendApp =  onOffHelper.Install(sNodes.Get(i));
            sendApp.Start(Seconds(0.0));
            sendApp.Stop(Seconds(10.0));

            Ptr<OnOffApplication> sender = StaticCast<OnOffApplication>(sendApp.Get(0));
            sender->TraceConnectWithoutContext("Tx", MakeCallback(&onOffTxHelper));
        }
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();
    Simulator::Destroy();

    int totalRecieved = 0;
    int packetReceived = 0;
    for(auto i: recieved_size){
        packetReceived += i.second / 1024;
        totalRecieved += i.second;
    }

    double throughput = totalRecieved*8/1e6 / 10;
    double delivery_ratio = packetReceived / (double)packetSent;
    NS_LOG_UNCOND("throughput : " << throughput);
    NS_LOG_UNCOND("Delivery Ratio : " << delivery_ratio);

    if(graph != -1){
        ofstream tpfile(throughputfilename, std::ios::app);
        ofstream drfile(drfilename, std::ios::app);

        if(graph == 0){
            tpfile << nNodes << "\t" << throughput << endl;
            drfile << nNodes << "\t" << delivery_ratio << endl;
        }
        else if(graph == 1){
            tpfile << nFlows << "\t" << throughput << endl;
            drfile << nFlows << "\t" << delivery_ratio << endl;
        }
        else if(graph == 2){
            tpfile << nPackets << "\t" << throughput << endl;
            drfile << nPackets << "\t" << delivery_ratio << endl;
        }
        else if(graph == 3){
            tpfile << coverageArea << "\t" << throughput << endl;
            drfile << coverageArea << "\t" << delivery_ratio << endl;
        }
    }

    return 0;
}
