#include <string>
#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("p2p_tcp_4dm1");

int
main (int argc, char *argv[])
{
  bool tracing = true;   
  bool enableFlowMonitor = true;
  uint32_t maxBytes = 0;
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing); 
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("maxBytes", "Total number of bytes for application to send", maxBytes);
  cmd.Parse (argc, argv);
  Time::SetResolution (Time::NS);
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1000));
  
  // Creating Nodes
  NodeContainer nodes;
  nodes.Create (6);
  
  NodeContainer n0n1 = NodeContainer (nodes.Get  (0), nodes.Get (1));
  NodeContainer n1n2 = NodeContainer (nodes.Get  (1), nodes.Get (2));
  NodeContainer n2n3 = NodeContainer (nodes.Get  (1), nodes.Get (3));
  NodeContainer n3n4 = NodeContainer (nodes.Get  (3), nodes.Get (4));
  NodeContainer n3n5 = NodeContainer (nodes.Get  (3), nodes.Get (5));
  
  
  // Assinging Channel
  PointToPointHelper p0p1;
  p0p1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p0p1.SetChannelAttribute ("Delay", StringValue ("5ms"));
  PointToPointHelper p1p2;
  p1p2.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p1p2.SetChannelAttribute ("Delay", StringValue ("5ms"));
  PointToPointHelper p1p3;
  p1p3.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  p1p3.SetChannelAttribute ("Delay", StringValue ("20ms"));

  PointToPointHelper p3p4;
  p3p4.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p3p4.SetChannelAttribute ("Delay", StringValue ("5ms")); 
 
  PointToPointHelper p3p5;
  p3p5.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p3p5.SetChannelAttribute ("Delay", StringValue ("5ms"));

  // Connecting Channel with NIC Card
  NetDeviceContainer d0d1 = p0p1.Install (n0n1);
  NetDeviceContainer d1d2 = p1p2.Install (n1n2);
  NetDeviceContainer d2d3 = p1p3.Install (n2n3);
  NetDeviceContainer d3d4 = p3p4.Install (n3n4);
  NetDeviceContainer d3d5 = p3p5.Install (n3n5);

  // Installing TCP/IP stack in the nodes
  InternetStackHelper stack;
  stack.Install (nodes);
  


  // Assiging IP address to the nodes
  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interface1 = address1.Assign (d0d1);

  Ipv4AddressHelper address2;
  address2.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interface2 = address2.Assign (d1d2);

  Ipv4AddressHelper address3;
  address3.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interface3 = address3.Assign (d2d3);
  
  Ipv4AddressHelper address4;
  address4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer interface4 = address4.Assign (d3d4);
  
  Ipv4AddressHelper address5;
  address5.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer interface5 = address5.Assign (d3d5);


// Create router nodes, initialize routing database and routing tables
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Adding Bulk Send Application and attaching to the Node 1
    NS_LOG_INFO ("Create Applications.");
    
  Ptr<Node> appSource1 = NodeList::GetNode (0);
  Ptr<Node> appSink1 = NodeList::GetNode (4);
  Ipv4Address remoteAddr = appSink1->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();


  // Creating a socket and Linking Source Aplication with Destination 

  OnOffHelper onoff ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr, 1)));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps = onoff.Install (nodes.Get  (0));
  apps.Start (Seconds (1));
  apps.Stop (Seconds (60));
  PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 1));
  apps = sink.Install (nodes.Get (4));
  
  Ptr<Node> appSource2 = NodeList::GetNode (2);
  Ptr<Node> appSink2 = NodeList::GetNode (5);
  Ipv4Address remoteAddr1 = appSink2->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();


  // Creating a socket and Linking Source Aplication with Destination 

  OnOffHelper onoff1 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr1,2)));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff1.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps1 = onoff1.Install (nodes.Get  (2));
  apps1.Start (Seconds (1));
  apps1.Stop (Seconds (60));
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (),2));
  apps1 = sink1.Install (nodes.Get (5));
 

  p0p1.EnablePcap ("dumbell_1",nodes.Get(0)->GetId(),0);
  p3p4.EnablePcap ("dumbell_2",nodes.Get(1)->GetId(),0);
  p1p2.EnablePcap ("dumbell_3",nodes.Get(2)->GetId(),0);
  p3p5.EnablePcap ("dumbell_4",nodes.Get(3)->GetId(),0);

  
// Installing Flow Monitor
  FlowMonitorHelper flowmonHelper;
  if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }
AnimationInterface anim("Dumbell-onoff.xml");
anim.SetConstantPosition (nodes.Get(0), 7,24);
anim.SetConstantPosition (nodes.Get(1), 24, 49);
anim.SetConstantPosition (nodes.Get(3), 73, 49);
anim.SetConstantPosition (nodes.Get(2), 8, 74);
anim.SetConstantPosition (nodes.Get(4), 97, 23);
anim.SetConstantPosition (nodes.Get(5), 93, 73);
  Simulator::Stop (Seconds (60));
  Simulator::Run ();

// Enabling the XML Flow Monitor
 if (enableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("Dumbell-onoff.flowmon", false, false);
    }
  Simulator::Destroy ();
  return 0;
}
