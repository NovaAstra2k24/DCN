#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/config-store-module.h"
#include "ns3/tcp-header.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  bool enableFlowMonitor = true;
  uint32_t maxBytes = 0;
  CommandLine cmd;
  cmd.Parse (argc, argv);
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
//  cmd.AddValue ("maxBytes",
   //             "Total number of bytes for application to send", maxBytes);
//

Time::SetResolution (Time::NS);
Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1000));

//  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
//LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (8);

NodeContainer n0n3 = NodeContainer (nodes.Get (0), nodes.Get (3));
NodeContainer n1n3 = NodeContainer (nodes.Get (1), nodes.Get (3));
NodeContainer n2n3 = NodeContainer (nodes.Get (2), nodes.Get (3));
NodeContainer n3n4 = NodeContainer (nodes.Get (3), nodes.Get (4));
NodeContainer n4n5 = NodeContainer (nodes.Get (4), nodes.Get (5));
NodeContainer n4n6 = NodeContainer (nodes.Get (4), nodes.Get (6));
NodeContainer n4n7 = NodeContainer (nodes.Get (4), nodes.Get (7));


PointToPointHelper p0p3;
p0p3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p0p3.SetChannelAttribute ("Delay", StringValue ("5ms"));
PointToPointHelper p1p3;
p1p3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p1p3.SetChannelAttribute ("Delay", StringValue ("5ms"));
PointToPointHelper p2p3;
p2p3.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p2p3.SetChannelAttribute ("Delay", StringValue ("5ms"));
PointToPointHelper p3p4;
p3p4.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
p3p4.SetChannelAttribute ("Delay", StringValue ("500ms"));
PointToPointHelper p4p5;
p4p5.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p4p5.SetChannelAttribute ("Delay", StringValue ("5ms"));
PointToPointHelper p4p6;
p4p6.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p4p6.SetChannelAttribute ("Delay", StringValue ("5ms"));
PointToPointHelper p4p7;
p4p7.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p4p7.SetChannelAttribute ("Delay", StringValue ("5ms"));


NetDeviceContainer d0d3 = p0p3.Install (n0n3);
NetDeviceContainer d1d3 = p1p3.Install (n1n3);
NetDeviceContainer d2d3 = p2p3.Install (n2n3);
NetDeviceContainer d3d4 = p3p4.Install (n3n4);
NetDeviceContainer d4d5 = p0p3.Install (n4n5);
NetDeviceContainer d4d6 = p1p3.Install (n4n6);
NetDeviceContainer d4d7 = p2p3.Install (n4n7);

InternetStackHelper stack;
stack.Install (nodes);




Ipv4AddressHelper address1;
address1.SetBase ("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface1 = address1.Assign (d0d3);

Ipv4AddressHelper address2;
address2.SetBase ("10.2.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface2 = address2.Assign (d1d3);

Ipv4AddressHelper address3;
address3.SetBase ("10.3.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface3 = address3.Assign (d2d3);

Ipv4AddressHelper address4;
address4.SetBase ("10.4.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface4 = address4.Assign (d3d4);

Ipv4AddressHelper address5;
address5.SetBase ("10.5.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface5 = address5.Assign (d4d5);

Ipv4AddressHelper address6;
address6.SetBase ("10.6.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface6 = address4.Assign (d4d6);

Ipv4AddressHelper address7;
address7.SetBase ("10.7.1.0", "255.255.255.0");
Ipv4InterfaceContainer interface7 = address7.Assign (d4d7);

Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//
// Fixing TCP source and destination node 0 to node 5

  Ptr<Node> appSource1 = NodeList::GetNode (0);
  Ptr<Node> appSink1 = NodeList::GetNode (5);
  Ipv4Address remoteAddr1 = appSink1->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();

// Fixing TCP source and destination for Bulk transfer Application

  BulkSendHelper source1 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr1, 1));
  source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer apps1 = source1.Install (nodes.Get  (0));
  apps1.Start (Seconds (1));
  apps1.Stop (Seconds (100));
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 1));
  apps1 = sink1.Install (nodes.Get (5));

// Fixing TCP source and destination node 1 to node 6

  Ptr<Node> appSource2 = NodeList::GetNode (1);
  Ptr<Node> appSink2 = NodeList::GetNode (6);
  Ipv4Address remoteAddr2 = appSink2->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();

// Fixing TCP source and destination for Bulk transfer Application

  BulkSendHelper source2 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr2, 1));
  source2.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer apps2 = source2.Install (nodes.Get  (1));
  apps2.Start (Seconds (1));
  apps2.Stop (Seconds (100));
  PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 1));
  apps2 = sink2.Install (nodes.Get (6));

// Fixing TCP source and destination node 2 to node 7

  Ptr<Node> appSource3 = NodeList::GetNode (2);
  Ptr<Node> appSink3 = NodeList::GetNode (7);
  Ipv4Address remoteAddr3 = appSink3->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();

// Fixing TCP source and destination for Bulk transfer Application

  BulkSendHelper source3 ("ns3::TcpSocketFactory", InetSocketAddress (remoteAddr3, 1));
  source3.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer apps3 = source3.Install (nodes.Get  (2));
  apps3.Start (Seconds (1));
  apps3.Stop (Seconds (100));
  PacketSinkHelper sink3 ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny (), 1));
  apps3 = sink3.Install (nodes.Get (7));

//********************************************************

  p0p3.EnablePcap ("p2pb_1",nodes.Get(0)->GetId(),0);
  p3p4.EnablePcap ("p2pb_4",nodes.Get(4)->GetId(),0);

 
 FlowMonitorHelper flowmonHelper;
 if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }
//  AnimationInterface anim ("dumbell_bulk.xml"); // Mandatory
  Simulator::Stop (Seconds (100));
  Simulator::Run ();
 if (enableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("dumbell_bulk.flowmon", false, false);
    }

  Simulator::Destroy ();
  return 0;
}
