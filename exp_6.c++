//Exp - 6 udp - server-client

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("P2P_UDP");

int main (int argc, char *argv[])
{
  bool enableFlowMonitor = true;
  CommandLine cmd;
  cmd.Parse (argc, argv);
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);
//

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (100.0));

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10000));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (100.0));
//
  pointToPoint.EnablePcap ("p2p_1",nodes.Get(0)->GetId(),0);
  pointToPoint.EnablePcap ("p2p_2",nodes.Get(1)->GetId(),0);

 
 FlowMonitorHelper flowmonHelper;
 if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }
  AnimationInterface anim ("p2p.xml"); // Mandatory NetAnim
  Simulator::Stop (Seconds (100));
  Simulator::Run ();
 if (enableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("p2p_udp.flowmon", false, false);
    }
  Simulator::Destroy ();
  return 0;
}
