/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/ipv4-address.h>
#include <ns3/lorawan-module.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/mobility-module.h>
#include <ns3/applications-module.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/node.h>
#include <ns3/packet.h>

#include <ns3/lorawan-spectrum-value-helper.h>
#include <ns3/spectrum-phy.h>
#include <ns3/spectrum-value.h>
#include <ns3/spectrum-model.h>

#include <ns3/lte-module.h>
#include <ns3/buildings-helper.h>

#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"

#include <ns3/netanim-module.h>
#include "ns3/config-store.h"
#include <iostream>
#include <iomanip>

using namespace ns3;

int main (int argc, char *argv[]) {

  /*************************/
  /* Simulation Parameters */
  /*************************/
  uint32_t randomSeed = 12345;
  uint32_t nLoraDevices = 1;
  uint32_t nNbDevices = 1;
  uint32_t nLoraGateways = 1;
  uint32_t nNbGateways = 1;
  //double discRadius = 5000.0;
  uint32_t nRuns = 1;
  double drCalcPerLimit = 0.01;
  uint32_t drCalcMethodIndex = 2;
  double drCalcFixedDRIndex = 0;
  double totalTime = 10.0;
  /*uint32_t usUnconfirmedDataNbRep = 1;
  uint32_t dsPacketSize = 21;
  bool dsDataGenerate = false;
  bool dsConfirmedData = false;
  double dsDataExpMean = -1;
  uint32_t usPacketSize = 21;
  uint32_t usMaxBytes = 0;
  bool usConfirmedData = false;
  double usDataPeriod = 600.0;
  bool usConfirmedData = false;
  bool verbose = false;
     */

  /*************************/
  /* Variables             */
  /*************************/
  NodeContainer loraEDNodesContainer;
  NodeContainer loraGWNodesContainer;
  NodeContainer nbEDNodesContainer;
  NodeContainer nbGWNodesContainer;

  NetDeviceContainer loraEDDevices;
  NetDeviceContainer loraGWDevices;
  NetDeviceContainer nbEDDevices;
  NetDeviceContainer nbGWDevices;

  CommandLine cmd;
  cmd.AddValue ("nloradevices", 
      "number of lorawan class a end devices nodes[default:30]", 
      nLoraDevices);
  cmd.AddValue ("nnbdevices", 
      "number of nb-ior end devices nodes[default:30]", 
      nNbDevices);
  cmd.AddValue ("nloragateways", 
      "number of lorawan gateways [default:1]", 
      nLoraGateways);
  cmd.AddValue ("nnbgateways", 
      "number of nb-iot gateways [default:1]", 
      nNbGateways);
  cmd.Parse (argc, argv);

  for (uint32_t i = 0; i < nRuns; i++) {
    uint32_t seed = randomSeed + i;
    SeedManager::SetSeed (seed);

    //setDRCalcPerLimit (drCalcPerLimit);
    //setDrCalcFixedDrIndex (drCalcFixedDRIndex);
    /* caseRun(nLoraDevices, nNbDevices, nLoraGateways, 
       nNbGateways, discRadius, totalTime,
       usPacketSize, usMaxBytes, usDataPeriod, 
       usUnconfirmedDataNbRep, usConfirmedData,
       dsPacketSize, dsDataGenerate, dsDataExpMean, dsConfirmedData,
       verbose);
       }*/

    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults ();

    if (!(drCalcPerLimit >= 0.0 && drCalcPerLimit <= 1.0)) {
      std::cout << " Invalid Data Rate Calculcation Method index provided: " 
        << (unsigned) drCalcMethodIndex << std::endl;
    }

    if (!(drCalcFixedDRIndex < LoRaWAN::m_supportedDataRates.size() - 1)) {
      std::cout << "Invalid DR index provided: " 
        << (uint32_t) drCalcFixedDRIndex << std::endl;
    }

    // Create LoRaWan GW and ED 
    /*std::cout << "Creating " << (unsigned) nLoraGateways << " LoRaWAN Gateways.\n";
    std::cout << "Creating " << (unsigned) nLoraDevices << " LoRaWAN class A end devices.\n";
    loraGWNodesContainer.Create(nLoraGateways);
    loraEDNodesContainer.Create(nLoraDevices);*/

    // Create NB-IoT eNB and ED
    std::cout << "Creating " << (unsigned) nNbDevices << " NB-IoT devices.\n";
    std::cout << "Creating " << (unsigned) nNbGateways << " NB-IoT Gateways.\n";
    nbGWNodesContainer.Create(nNbGateways);
    nbEDNodesContainer.Create(nNbDevices);

    // Set mobility for all nodes
    /*MobilityHelper endDeviceMobility;
    endDeviceMobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
        "X", DoubleValue (0.0),
        "Y", DoubleValue (0.0),
        "rho", DoubleValue (discRadius));
    endDeviceMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    endDeviceMobility.Install (loraEDNodesContainer);
    endDeviceMobility.Install (nbEDNodesContainer);

    Ptr<ListPositionAllocator> nodePositionList = CreateObject<ListPositionAllocator> ();
    Vector v1 = Vector(0.0, 0.0, 0.0);
    Vector v2 = Vector(-discRadius/2 + 1*discRadius, 0.0, 0.0);
    nodePositionList->Add (v1);
    nodePositionList->Add (v2);

    MobilityHelper gatewayMobility;
    gatewayMobility.SetPositionAllocator (nodePositionList);
    gatewayMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    gatewayMobility.Install (loraGWNodesContainer);
    gatewayMobility.Install (nbGWNodesContainer);*/

    uint16_t numberOfNodes = /*nLoraDevices + nLoraGateways + */nNbDevices + nNbGateways;
    double distance = 60.0;
    Ptr<ListPositionAllocator> nodePositionList = CreateObject<ListPositionAllocator> ();
    for (uint16_t i = 0; i < numberOfNodes; i++) {
      nodePositionList->Add (Vector(distance * i, 0, 0));
    }
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(nodePositionList);
    //mobility.Install(loraEDNodesContainer);
    //mobility.Install(loraGWNodesContainer);
    mobility.Install(nbEDNodesContainer);
    mobility.Install(nbGWNodesContainer);

    //Create lora Devices
    /*LoRaWANHelper lorawanHelper;
    lorawanHelper.SetNbRep (usUnconfirmedDataNbRep);
    loraEDDevices = lorawanHelper.Install (loraEDNodesContainer);
    lorawanHelper.SetDeviceType (LORAWAN_DT_GATEWAY);
    loraGWDevices = lorawanHelper.Install (loraGWNodesContainer);*/
    //lorawanHelper.EnableLogComponents (LOG_LEVEL_INFO);

    // Create NB-IoT Devices
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> (); 
    nbGWDevices = lteHelper->InstallEnbDevice (nbGWNodesContainer);
    nbEDDevices = lteHelper->InstallUeDevice (nbEDNodesContainer);
    lteHelper->EnableTraces ();

    //Install LoRa Applications
    // PacketSocketHelper::Install adds a PacketSocketFactory 
    // object as an aggregate object to each of the nodes in the NodeContainer
    // This factory object is needed when applications create sockets in StartApplication
    /*PacketSocketHelper loraPacketSocket;
    loraPacketSocket.Install(loraEDNodesContainer);
    loraPacketSocket.Install(loraGWNodesContainer);

    // Gateways and Network Server:
    ApplicationContainer loraGatewayApps;
    ObjectFactory m_factory2; 
    m_factory2.SetTypeId ("ns3::LoRaWANGatewayApplication");
    for (NodeContainer::Iterator i = loraGWNodesContainer.Begin (); 
        i != loraGWNodesContainer.End (); ++i) {
      Ptr<Node> node = *i;
      Ptr<Application> app = m_factory2.Create<Application> ();
      node->AddApplication (app);
      loraGatewayApps.Add (app);
    }

    loraGatewayApps.Start (Seconds (0.0));
    loraGatewayApps.Stop (Seconds (totalTime));

    // Set attributes on LoRaWANNetworkServer object:
    Ptr<LoRaWANNetworkServer> lorawanNSPtr = LoRaWANNetworkServer::getLoRaWANNetworkServerPointer ();
    NS_ASSERT (lorawanNSPtr);
    if (lorawanNSPtr) {
      lorawanNSPtr->SetAttribute ("PacketSize", UintegerValue (dsPacketSize));
      lorawanNSPtr->SetAttribute ("GenerateDataDown", BooleanValue (dsDataGenerate));
      lorawanNSPtr->SetAttribute ("ConfirmedDataDown", BooleanValue (dsConfirmedData));
      std::stringstream downstreamiatss;
      downstreamiatss << "ns3::ExponentialRandomVariable[Mean=" << dsDataExpMean << "]";
      lorawanNSPtr->SetAttribute ("DownstreamIAT", StringValue (downstreamiatss.str ()));
    }

    // End devices:
    ApplicationContainer loraEndDeviceApp;
    ObjectFactory m_factory; 
    m_factory.SetTypeId ("ns3::LoRaWANEndDeviceApplication");
    // Attributes shared by all end device apps:
    m_factory.Set ("PacketSize", UintegerValue (usPacketSize));
    m_factory.Set ("MaxBytes", UintegerValue (usMaxBytes));
    m_factory.Set ("ConfirmedDataUp", BooleanValue (usConfirmedData));
    std::stringstream upstreamiatss;
    upstreamiatss << "ns3::ConstantRandomVariable[Constant=" << usDataPeriod << "]";
    m_factory.Set ("UpstreamIAT", StringValue (upstreamiatss.str ()));
    // Limit the number of channels
    m_factory.Set ("ChannelRandomVariable", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

    Ptr<UniformRandomVariable> appStartRandomVariable = CreateObject<UniformRandomVariable> ();
    for (NodeContainer::Iterator i = loraEDNodesContainer.Begin (); 
        i != loraEDNodesContainer.End (); ++i) {
      Ptr<Node> node = *i;
      Ptr<Application> app = m_factory.Create<Application> ();
      node->AddApplication (app);

      // Start the end device apps at different times:
      double appStartRandomValue = appStartRandomVariable->GetValue (0, usDataPeriod);
      app->SetStartTime (Seconds (appStartRandomValue));
      app->SetStopTime(Seconds (totalTime));

      loraEndDeviceApp.Add (app);
    }

    // Assign data rate indexes to end device applications:
    for (ApplicationContainer::Iterator aci = loraEndDeviceApp.Begin (); 
        aci != loraEndDeviceApp.End (); ++aci) {
      Ptr<Application> app = *aci;
      app->SetAttribute ("DataRateIndex", UintegerValue(drCalcFixedDRIndex));
    }*/

    // Install NB-IoT Applications
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
    lteHelper->SetEpcHelper (epcHelper);

    InternetStackHelper internet;
    Ptr<Node> pgw = epcHelper->GetPgwNode ();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create (1);
    Ptr<Node> remoteHost = remoteHostContainer.Get (0);
    internet.Install (remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
    NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
    // interface 0 is localhost, 1 is the p2p device
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
    remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

    // Install the IP stack on the UEs
    internet.Install (nbEDNodesContainer);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (nbEDDevices);
    // Assign IP address to UEs, and install applications
    for (uint32_t u = 0; u < nbEDNodesContainer.GetN (); ++u) {
      Ptr<Node> ueNode = nbEDNodesContainer.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

    // Attach one UE per eNodeB
    for (uint16_t i = 0; i < nNbDevices; i++) {
      lteHelper->Attach (nbEDDevices.Get(i), nbGWDevices.Get(0));
      // side effect: the default EPS bearer will be activated
    }
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer (q);
    lteHelper->ActivateDataRadioBearer (nbEDDevices, bearer);

    // Install and start applications on UEs and remote host
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;
    uint16_t otherPort = 3000;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    for (uint32_t u = 0; u < nbEDNodesContainer.GetN (); ++u) {
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
      serverApps.Add (dlPacketSinkHelper.Install (nbEDNodesContainer.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
      serverApps.Add (packetSinkHelper.Install (nbEDNodesContainer.Get(u)));

      double interPacketInterval = 100;
      UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

      UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      client.SetAttribute ("MaxPackets", UintegerValue(1000000));

      clientApps.Add (dlClient.Install (remoteHost));
      clientApps.Add (ulClient.Install (nbEDNodesContainer.Get(u)));
      if (u+1 < nbEDNodesContainer.GetN ()) {
        clientApps.Add (client.Install (nbEDNodesContainer.Get(u+1)));
      } else {
        clientApps.Add (client.Install (nbEDNodesContainer.Get(0)));
      }
    }
    serverApps.Start (Seconds (0.01));
    clientApps.Start (Seconds (0.01));

    // Start Simulation
    std::cout << "Starting simulation for " << totalTime << " s ...\n";
    Simulator::Stop (Seconds (totalTime));
    
    // netamin:
    AnimationInterface anim ("lorawan-netamin.xml");
    anim.EnablePacketMetadata (true);

    Simulator::Run ();
    Simulator::Destroy ();

  }

  std::cout << "End Simulation\n";
  return 0;
}


