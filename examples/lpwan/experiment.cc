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

NS_LOG_COMPONENT_DEFINE ("experiment");

typedef enum
{
  LORAWAN_DR_CALC_METHOD_PER_INDEX = 0x00,
  LORAWAN_DR_CALC_METHOD_RANDOM_INDEX = 0x01,
  LORAWAN_DR_CALC_METHOD_FIXED_INDEX = 0x02,
} LoRaWANDataRateCalcMethodIndex;

class LoRaWANExampleTracing
{
public:
  LoRaWANExampleTracing();
  void CaseRun (
      NodeContainer loraEDNodeContainer,
      NodeContainer loraGWNodeContainer,
      NetDeviceContainer loraEDDevices,
      NetDeviceContainer loraGWDevices,
      uint32_t nLoraDevices,
      uint32_t nLoraGateways,
      double discRadius,
      double totalTime,
      uint32_t usPacketSize,
      uint32_t usMaxBytes,
      uint32_t usUnconfirmedDataNbRep,
      bool usConfirmedData,
      uint32_t dsPacketSize,
      bool dsDataGenerate,
      double dsDataExpMean,
      bool dsConfirmedData,
      bool tracePhyTransmissions,
      bool tracePhyStates,
      bool traceMacPackets,
      bool traceMacStates,
      bool traceEdMsgs,
      bool traceNsDsMsgs,
      bool traceMisc,
      std::string phyTransmissionTraceCSVFileName,
      std::string phyStateTraceCSVFileName,
      std::string macPacketTraceCSVFileName,
      std::string macStateTraceCSVFileName,
      std::string edMsgTraceCSVFileName,
      std::string nsDSMsgTraceCSVFileName,
      std::string miscTraceCSVFileName,
      std::string nodesCSVFileName);

  void LogOutputLine (std::string output, std::string);

  std::ostringstream GeneratePhyTraceOutputLine (
      std::string traceSource, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> packet, 
      bool insertNewLine = true);

  static void PhyTxBegin (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> packet);

  static void PhyTxEnd (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> packet);

  static void PhyTxDrop (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> packet);

  static void PhyRxBegin (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> packet);

  static void PhyRxEnd (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> 
      packet, 
      double lqi);

  static void PhyRxDrop (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      Ptr<const Packet> packet, 
      LoRaWANPhyDropRxReason dropReason);

  std::ostringstream GenerateMacTraceOutputLine (
      std::string traceSource, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet, 
      bool insertNewLine = true);

  static void MacTx (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet);

  static void MacTxOk(
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet);

  static void MacTxDrop(
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet);

  static void MacRx(
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet);

  static void MacRxDrop (LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet);

  static void MacSentPkt (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      Ptr<const Packet> packet, 
      uint8_t n_transmissions);

  static void PhyStateChangeNotification (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANPhy> phy, 
      LoRaWANPhyEnumeration oldState, 
      LoRaWANPhyEnumeration newState);

  static void MacStateChangeNotification (
      LoRaWANExampleTracing* example, 
      Ptr<LoRaWANNetDevice> device, 
      Ptr<LoRaWANMac> mac, 
      LoRaWANMacState oldState, 
      LoRaWANMacState newState);

  static void nrRW1SentTrace (
      LoRaWANExampleTracing* example, 
      uint32_t oldValue, 
      uint32_t newValue);

  static void nrRW2SentTrace (
      LoRaWANExampleTracing* example, 
      uint32_t oldValue, 
      uint32_t newValue);

  static void nrRW1MissedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t oldValue, 
      uint32_t newValue);

  static void nrRW2MissedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t oldValue, 
      uint32_t newValue);

  std::ostringstream GenerateEDMsgTraceOutputLine (
      std::string traceSource, 
      uint32_t deviceAddress, 
      uint8_t msgType, 
      Ptr<const Packet> packet, 
      bool insertNewLine = true);

  static void USMsgTransmittedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t deviceAddress, 
      uint8_t msgType,
      Ptr<const Packet> packet);
  
  static void DSMsgReceivedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t deviceAddress, 
      uint8_t msgType, 
      Ptr<const Packet> packet, 
      uint8_t rw);

  std::ostringstream GenerateNSDSMsgTraceOutputLine (
      std::string traceSource, 
      uint32_t deviceAddress, 
      uint8_t transmissionsRemaning, 
      uint8_t msgType, 
      Ptr<const Packet> packet, 
      bool insertNewLine = true);

  static void DSMsgGeneratedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t, 
      uint8_t, 
      uint8_t, 
      Ptr<const Packet>);

  static void DSMsgTransmittedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t, 
      uint8_t, 
      uint8_t, 
      Ptr<const Packet>, 
      uint8_t rw);

  static void DSMsgAckdTrace (
      LoRaWANExampleTracing* example, 
      uint32_t, 
      uint8_t, 
      uint8_t, 
      Ptr<const Packet>);

  static void DSMsgDroppedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t, 
      uint8_t, 
      uint8_t, 
      Ptr<const Packet>);

  static void USMsgReceivedTrace (
      LoRaWANExampleTracing* example, 
      uint32_t deviceAddress, 
      uint8_t msgType, 
      Ptr<const Packet> packet);

  void WriteMiscStatsToFile ();

private:
  uint32_t m_nLoraEndDevices;
  uint32_t m_nLoraGateways;
  uint32_t m_nNbEndDevices;
  uint32_t m_nNbGateways;

  double m_discRadius;
  double m_totalTime;

  LoRaWANDataRateCalcMethodIndex m_drCalcMethodIndex;
  double m_drCalcPerLimit;
  uint8_t m_fixedDataRateIndex;

  uint32_t m_usPacketSize;
  uint32_t m_usMaxBytes;
  double m_usDataPeriod; // <! Period between subsequent data transmission of end devices
  uint32_t m_usUnconfirmedDataNbRep;
  bool m_usConfirmdData;

  uint32_t m_dsPacketSize;
  bool m_dsDataGenerate;
  double m_dsDataExpMean;
  bool m_dsConfirmedData;

  bool m_verbose;
  bool m_stdcout;
  std::string m_phyTransmissionTraceCSVFileName;
  std::string m_phyStateTraceCSVFileName;
  std::string m_macPacketTraceCSVFileName;
  std::string m_macStateTraceCSVFileName;
  std::string m_edMsgTraceCSVFileName;
  std::string m_nsDSMsgTraceCSVFileName;
  std::string m_miscTraceCSVFileName;
  std::map<std::string, std::ofstream> output_streams;

  std::string m_nodesCSVFileName;

  uint32_t m_nrRW1Sent;
  uint32_t m_nrRW2Sent;
  uint32_t m_nrRW1Missed;
  uint32_t m_nrRW2Missed;

  NodeContainer m_loraEDNodeContainer;
  NodeContainer m_loraGWNodeContainer;
  NodeContainer m_nbEDNodeContainer;
  NodeContainer m_nbGWNodeContainer;
 
  NetDeviceContainer m_loraEDDevices;
  NetDeviceContainer m_loraGWDevices;
  NetDeviceContainer m_nbEDDevices;
  NetDeviceContainer m_nbGWDevices;


private:
  void SetupTracing (
      bool tracePhyTransmissions, 
      bool tracePhyStates, 
      bool traceMacPackets, 
      bool traceMacStates, 
      bool traceEdMsgs, 
      bool traceNsDsMsgs, 
      bool traceMisc);

  void InstallApplications ();

  void OutputNodesToFile ();
};


int main (int argc, char *argv[]) {

  /*************************/
  /* Simulation Parameters */
  /*************************/
  uint32_t randomSeed = 12345;
  uint32_t nLoraDevices = 100;
  uint32_t nNbDevices = 100;
  uint32_t nLoraGateways = 2;
  uint32_t nNbGateways = 2;
  double discRadius = 5000.0;
  uint32_t nRuns = 5;
  double drCalcPerLimit = 0.01;
  uint32_t drCalcMethodIndex = 2;
  double drCalcFixedDRIndex = 0;
  double totalTime = 300.0;
  uint32_t usUnconfirmedDataNbRep = 1;
  uint32_t dsPacketSize = 21;
  bool dsDataGenerate = false;
  bool dsConfirmedData = true;
  double dsDataExpMean = -1;
  uint32_t usPacketSize = 21;
  uint32_t usMaxBytes = 0;
  bool usConfirmedData = false;
  double usDataPeriod = 50.0;
  bool tracePhyTransmissions = true;
  bool tracePhyStates = true;
  bool traceMacPackets = true;
  bool traceMacStates = true;
  bool traceEdMsgs = true;
  bool traceNsDsMsgs = true;
  bool traceMisc = true;
  std::string outputFileNamePrefix = "output/LoRaWAN-example-tracing";

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
  cmd.AddValue (
      "nloradevices", 
      "number of lorawan class a end devices nodes[default:30]", 
      nLoraDevices);

  cmd.AddValue (
      "nnbdevices", 
      "number of nb-ior end devices nodes[default:30]", 
      nNbDevices);

  cmd.AddValue (
      "nloragateways", 
      "number of lorawan gateways [default:1]", 
      nLoraGateways);

  cmd.AddValue (
      "nnbgateways", 
      "number of nb-iot gateways [default:1]", 
      nNbGateways);

  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue (320));

  std::time_t unix_epoch = std::time(nullptr);
  for (uint32_t i = 0; i < nRuns; i++) {
    uint32_t seed = randomSeed + i;
    SeedManager::SetSeed (seed);
    std::cout << "Run " << (unsigned) i << "\n";

    std::ostringstream simRunFilesPrefix;
    simRunFilesPrefix << outputFileNamePrefix << "-" << unix_epoch << "-" << std::to_string(i);

    // prep csv files:
    std::ostringstream phyTransmissionTraceCSVFileName;
    phyTransmissionTraceCSVFileName << simRunFilesPrefix.str() << "-trace-phy-tx.csv";
    if (tracePhyTransmissions) {
      std::ofstream out (phyTransmissionTraceCSVFileName.str ().c_str ());
      out << "Time," <<
        "DeviceType," <<
        "NodeId," <<
        "IfIndex," <<
        "PhyIndex," <<
        "TraceSource," <<
        "PhyTraceIdTag," <<
        "PacketHex," <<
        "PacketLength," <<
        "Miscellaneous" <<
        std::endl;
      out.close ();
    }

    std::ostringstream phyStateTraceCSVFileName;
    phyStateTraceCSVFileName << simRunFilesPrefix.str() << "-trace-phy-state.csv";
    if (tracePhyStates) {
      std::ofstream out2 (phyStateTraceCSVFileName.str ().c_str ());
      out2 << "Time," <<
        "DeviceType," <<
        "NodeId," <<
        "IfIndex," <<
        "PhyIndex," <<
        "TraceSource," <<
        "OldState," <<
        "NewState" <<
        std::endl;
      out2.close ();
    }

    std::ostringstream macPacketTraceCSVFileName;
    macPacketTraceCSVFileName << simRunFilesPrefix.str() << "-trace-mac-packets.csv";
    if (traceMacPackets) {
      std::ofstream out (macPacketTraceCSVFileName.str ().c_str ());
      // TODO
      out << "Time," <<
        "DeviceType," <<
        "NodeId," <<
        "IfIndex," <<
        "PhyIndex," <<
        "TraceSource," <<
        "PhyTraceIdTag," <<
        "PacketHex," <<
        "PacketLength," <<
        "Miscellaneous" <<
        std::endl;
      out.close ();
    }

    std::ostringstream macStateTraceCSVFileName;
    macStateTraceCSVFileName << simRunFilesPrefix.str() << "-trace-mac-state.csv";
    if (traceMacStates) {
      std::ofstream out3 (macStateTraceCSVFileName.str ().c_str ());
      out3 << "Time," <<
        "DeviceType," <<
        "NodeId," <<
        "IfIndex," <<
        "MacIndex," <<
        "TraceSource," <<
        "OldState," <<
        "NewState" <<
        std::endl;
      out3.close ();
    }

    std::ostringstream edMsgTraceCSVFileName;
    edMsgTraceCSVFileName << simRunFilesPrefix.str() << "-trace-ed-msgs.csv";
    if (traceEdMsgs) {
      std::ofstream outed (edMsgTraceCSVFileName.str ().c_str ());
      outed << "Time," <<
        "TraceSource," <<
        "NodeId," <<
        "MsgType," <<
        "PacketHex," <<
        "PacketLength" <<
        std::endl;
      outed.close ();
    }

    std::ostringstream nsDSMsgTraceCSVFileName;
    nsDSMsgTraceCSVFileName << simRunFilesPrefix.str() << "-trace-ns-dsmsgs.csv";
    if (traceNsDsMsgs) {
      std::ofstream outns (nsDSMsgTraceCSVFileName.str ().c_str ());
      outns << "Time," <<
        "TraceSource," <<
        "NodeId," <<
        "MsgType," <<
        "TxRemaining," <<
        "PacketHex," <<
        "PacketLength" <<
        std::endl;
      outns.close ();
    }

    std::ostringstream miscTraceCSVFileName;
    miscTraceCSVFileName << simRunFilesPrefix.str() << "-trace-misc.csv";
    if (traceMisc) {
      std::ofstream out4 (miscTraceCSVFileName.str ().c_str ());
      out4 << "nrRW1Sent," <<
        "nrRW2Sent," <<
        "nrRW1Missed," <<
        "nrRW2Missed" <<
        std::endl;
      out4.close ();
    }

    std::ostringstream nodesCSVFileName;
    nodesCSVFileName << simRunFilesPrefix.str() << "-nodes.csv";
    std::ofstream out5 (nodesCSVFileName.str ().c_str ());
    out5 << "DeviceType," <<
      "NodeId," <<
      "Position x," <<
      "Position y," <<
      "DistanceToClosestGW," <<
      "DataRateIndex" <<
      std::endl;
    out5.close ();

    std::cout << std::endl;

    std::ostringstream simSettings;
    simSettings << "Simulation settings:" << std::endl;
    simSettings << "\trandomSeed = " << randomSeed << std::endl;
    simSettings << "\tnLoraEndDevices = " << nLoraDevices << std::endl;
    simSettings << "\tnLoraGateways = " << nLoraGateways << std::endl;
    simSettings << "\tnNbEndDevices = " << nNbDevices << std::endl;
    simSettings << "\tnNbGateways = " << nNbGateways << std::endl;
    simSettings << "\tdiscRadius = " << discRadius << std::endl;
    simSettings << "\ttotalTime = " << totalTime << std::endl;
    simSettings << "\tnRuns = " << nRuns << std::endl;
    simSettings << "\tusPacketSize = " << usPacketSize << std::endl;
    simSettings << "\tusMaxBytes = " << usMaxBytes << std::endl;
    simSettings << "\tusDataPeriod = " << usDataPeriod << std::endl;
    simSettings << "\tusUnconfirmedDataNbRep = " << usUnconfirmedDataNbRep << std::endl;
    simSettings << "\tusConfirmedData = " << usConfirmedData << std::endl;
    simSettings << "\tdsPacketSize = " << dsPacketSize << std::endl;
    simSettings << "\tdsDataGenerate = " << dsDataGenerate << std::endl;
    simSettings << "\tdsDataExpMean = " << dsDataExpMean << std::endl;
    simSettings << "\tdsConfirmedData = " << dsConfirmedData << std::endl;
        simSettings << "\ttracePhyTransmissions = " << tracePhyTransmissions << std::endl;
    simSettings << "\ttracePhyStates = " << tracePhyStates << std::endl;
    simSettings << "\ttraceMacPackets = " << traceMacPackets << std::endl;
    simSettings << "\ttraceMacStates = " << traceMacStates << std::endl;
    simSettings << "\ttraceEdMsgs = " << traceEdMsgs << std::endl;
    simSettings << "\ttraceNsDsMsgs = " << traceNsDsMsgs << std::endl;
    simSettings << "\ttraceMisc = " << traceMisc << std::endl;
    simSettings << "\toutputFileNamePrefix = " << outputFileNamePrefix << std::endl;
    simSettings << "\trun = " << i << std::endl;
    simSettings << "\tseed = " << seed << std::endl;
    simSettings << "\tphyTransmissionTraceCSVFileName = " << phyTransmissionTraceCSVFileName.str() << std::endl;
    simSettings << "\tphyStateTraceCSVFileName = " << phyStateTraceCSVFileName.str() << std::endl;
    simSettings << "\tmacPacketTraceCSVFileName = " << macPacketTraceCSVFileName.str() << std::endl;
    simSettings << "\tmacStateTraceCSVFileName = " << macStateTraceCSVFileName.str() << std::endl;
    simSettings << "\tedMsgTraceCSVFileName = " << edMsgTraceCSVFileName.str() << std::endl;
    simSettings << "\tnsDSMsgTraceCSVFileName = " << nsDSMsgTraceCSVFileName.str() << std::endl;
    simSettings << "\tmiscTraceCSVFileName = " << miscTraceCSVFileName.str() << std::endl;
    simSettings << "\tnodesCSVFileName = " << nodesCSVFileName.str() << std::endl;
    //simSettings << "\tData rate assignment method index: " << loRaWANDataRateCalcMethodIndex;
    //if (loRaWANDataRateCalcMethodIndex == LORAWAN_DR_CALC_METHOD_PER_INDEX)
    //  simSettings << ", PER limit = " << drCalcPerLimit << ", PER Packet size = " << (unsigned)LoRaWANExampleTracing::m_perPacketSize << " bytes";
    //else if (loRaWANDataRateCalcMethodIndex == LORAWAN_DR_CALC_METHOD_FIXED_INDEX)
    // simSettings << ", Fixed Data Rate Index = " << drCalcFixedDRIndex;
    simSettings  << std::endl;

    // print settings to std cout
    std::cout << simSettings.str();
    // write settings to file:
    std::ostringstream simSettingsFileName;
    simSettingsFileName << simRunFilesPrefix.str() << "-sim-settings.txt";
    std::ofstream out6 (simSettingsFileName.str ().c_str ());
    out6 << simSettings.str();
    out6.close ();

    if (dsDataExpMean == -1)
      dsDataExpMean = 10 * usDataPeriod; // set default value for dsDataExpMean

    LoRaWANExampleTracing example = LoRaWANExampleTracing ();
    //setDRCalcPerLimit (drCalcPerLimit);
    //setDrCalcFixedDrIndex (drCalcFixedDRIndex);
  
    ConfigStore inputConfig;
    inputConfig.ConfigureDefaults ();
    //Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (true));
    //Config::SetDefault("ns3::LteHelper::QRxLevMin", IntegerValue(-22));

    if (!(drCalcPerLimit >= 0.0 && drCalcPerLimit <= 1.0)) {
      std::cout << " Invalid Data Rate Calculcation Method index provided: " 
        << (unsigned) drCalcMethodIndex << std::endl;
    }

    if (!(drCalcFixedDRIndex < LoRaWAN::m_supportedDataRates.size() - 1)) {
      std::cout << "Invalid DR index provided: " 
        << (uint32_t) drCalcFixedDRIndex << std::endl;
    }


    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> (); 
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
    lteHelper->SetEpcHelper (epcHelper);
    Ptr<Node> pgw = epcHelper->GetPgwNode ();

    // Create a single RemoteHost
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create (1);
    Ptr<Node> remoteHost = remoteHostContainer.Get (0);
    InternetStackHelper internet;
    internet.Install (remoteHostContainer);

    // Create the Internet
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("1Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
    NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
    // interface 0 is localhost, 1 is the p2p device
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting = 
      ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());

    remoteHostStaticRouting->AddNetworkRouteTo (
        Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

    // Create LoRaWan GW and ED 
    std::cout << "Creating " << (unsigned) nLoraGateways << " LoRaWAN Gateways.\n";
    std::cout << "Creating " << (unsigned) nLoraDevices << " LoRaWAN class A end devices.\n";
    loraGWNodesContainer.Create(nLoraGateways);
    loraEDNodesContainer.Create(nLoraDevices);

    // Create NB-IoT eNB and ED
    std::cout << "Creating " << (unsigned) nNbDevices << " NB-IoT devices.\n";
    std::cout << "Creating " << (unsigned) nNbGateways << " NB-IoT Gateways.\n";
    nbGWNodesContainer.Create(nNbGateways);
    nbEDNodesContainer.Create(nNbDevices);

    // Set mobility for all nodes
    MobilityHelper loraMobility;
    loraMobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
        "X", DoubleValue (0.0),
        "Y", DoubleValue (0.0),
        "rho", DoubleValue (discRadius));
    loraMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    loraMobility.Install (loraEDNodesContainer);

    MobilityHelper nbMobility;
    nbMobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
        "X", DoubleValue (0.0),
        "Y", DoubleValue (0.0),
        "rho", DoubleValue (discRadius));
    nbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    nbMobility.Install (nbEDNodesContainer);


    Ptr<ListPositionAllocator> loraPositionList = CreateObject<ListPositionAllocator> ();
    for (uint32_t i = 0; i < (nLoraDevices); i++) {
      Vector v = Vector (-discRadius/2 + (i/2)*discRadius, -discRadius/2 + (i % 2)*discRadius, 0.0);
      loraPositionList->Add (v);
    }

    MobilityHelper loraGatewayMobility;
    loraGatewayMobility.SetPositionAllocator (loraPositionList);
    loraGatewayMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    loraGatewayMobility.Install (loraGWNodesContainer);

    Ptr<ListPositionAllocator> nbPositionList = CreateObject<ListPositionAllocator> ();
    for (uint32_t i = 0; i < (nNbDevices); i++) {
      Vector v = Vector (-discRadius/2 + (i/2)*discRadius, -discRadius/2 + (i % 2)*discRadius, 0.0);
      nbPositionList->Add (v);
    }

    MobilityHelper nbGatewayMobility;
    nbGatewayMobility.SetPositionAllocator (nbPositionList);
    nbGatewayMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    nbGatewayMobility.Install (nbGWNodesContainer);

    //Create lora Devices
    LoRaWANHelper lorawanHelper;
    lorawanHelper.SetNbRep (usUnconfirmedDataNbRep);
    loraEDDevices = lorawanHelper.Install (loraEDNodesContainer);
    lorawanHelper.SetDeviceType (LORAWAN_DT_GATEWAY);
    loraGWDevices = lorawanHelper.Install (loraGWNodesContainer);
    //lorawanHelper.EnableLogComponents (LOG_LEVEL_INFO);

    // Create NB-IoT Devices
    nbGWDevices = lteHelper->InstallEnbDevice (nbGWNodesContainer);
    nbEDDevices = lteHelper->InstallUeDevice (nbEDNodesContainer);
    lteHelper->EnableTraces ();

    //Install LoRa Applications
    // PacketSocketHelper::Install adds a PacketSocketFactory 
    // object as an aggregate object to each of the nodes in the NodeContainer
    // This factory object is needed when applications create sockets in StartApplication
    PacketSocketHelper loraPacketSocket;
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
    Ptr<LoRaWANNetworkServer> lorawanNSPtr = 
      LoRaWANNetworkServer::getLoRaWANNetworkServerPointer ();

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
    m_factory.Set ("ChannelRandomVariable", 
        StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

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
    }

    // Install the IP stack on the UEs
    internet.Install (nbEDNodesContainer);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (nbEDDevices));
    // Assign IP address to UEs, and install applications
    for (uint32_t u = 0; u < nbEDNodesContainer.GetN (); ++u) {
      Ptr<Node> ueNode = nbEDNodesContainer.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = 
        ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());

      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

    // Attach one UE per eNodeB
    uint32_t half = nNbDevices / 2;
    for (uint16_t i = 0; i < nNbDevices; i++) {
      if (half < i)
        lteHelper->Attach (nbEDDevices.Get(i), nbGWDevices.Get(0));
      else
        lteHelper->Attach (nbEDDevices.Get(i), nbGWDevices.Get(0));
      // side effect: the default EPS bearer will be activated
    }

    // Install and start applications on UEs and remote host
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;
    uint16_t otherPort = 3000;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
    for (uint32_t u = 0; u < nbEDNodesContainer.GetN (); ++u) {
      
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", 
          InetSocketAddress (Ipv4Address::GetAny (), dlPort));

      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", 
          InetSocketAddress (Ipv4Address::GetAny (), ulPort));

      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", 
          InetSocketAddress (Ipv4Address::GetAny (), otherPort));

      serverApps.Add (dlPacketSinkHelper.Install (nbEDNodesContainer.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
      serverApps.Add (packetSinkHelper.Install (nbEDNodesContainer.Get(u)));

      double interPacketInterval = 5000;
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

    example.CaseRun(
        loraEDNodesContainer,
        loraGWNodesContainer,
        loraEDDevices,
        loraGWDevices,
        nLoraDevices,
        nLoraGateways, 
        discRadius, 
        totalTime,  
        usPacketSize, 
        usMaxBytes, 
        usUnconfirmedDataNbRep, 
        usConfirmedData,
        dsPacketSize, 
        dsDataGenerate, 
        dsDataExpMean, 
        dsConfirmedData,
        tracePhyTransmissions, 
        tracePhyStates, 
        traceMacPackets, 
        traceMacStates, 
        traceEdMsgs, 
        traceNsDsMsgs, 
        traceMisc,
        phyTransmissionTraceCSVFileName.str (), 
        phyStateTraceCSVFileName.str (), 
        macPacketTraceCSVFileName.str (), 
        macStateTraceCSVFileName.str (), 
        edMsgTraceCSVFileName.str(), 
        nsDSMsgTraceCSVFileName.str(),  
        miscTraceCSVFileName.str(), 
        nodesCSVFileName.str());

    // Start Simulation
    std::cout << "Starting simulation for " << totalTime << " s ...\n";
    Simulator::Stop (Seconds (totalTime));
    
    // netamin:
    AnimationInterface anim ("lorawan-netamin.xml");
    anim.EnablePacketMetadata (true);

    Simulator::Run ();
    Simulator::Destroy ();
    example.WriteMiscStatsToFile ();
  }

  std::cout << "End Simulation\n";
  return 0;
}

LoRaWANExampleTracing::LoRaWANExampleTracing () : 
  m_nrRW1Sent(0), m_nrRW2Sent(0), m_nrRW1Missed(0), m_nrRW2Missed(0) {}

void
LoRaWANExampleTracing::CaseRun (
      NodeContainer loraEDNodeContainer,
      NodeContainer loraGWNodeContainer,
      NetDeviceContainer loraEDDevices,
      NetDeviceContainer loraGWDevices,
      uint32_t nLoraDevices,
      uint32_t nLoraGateways,
      double discRadius,
      double totalTime,
      uint32_t usPacketSize,
      uint32_t usMaxBytes,
      uint32_t usUnconfirmedDataNbRep,
      bool usConfirmedData,
      uint32_t dsPacketSize,
      bool dsDataGenerate,
      double dsDataExpMean,
      bool dsConfirmedData,
      bool tracePhyTransmissions,
      bool tracePhyStates,
      bool traceMacPackets,
      bool traceMacStates,
      bool traceEdMsgs,
      bool traceNsDsMsgs,
      bool traceMisc,
      std::string phyTransmissionTraceCSVFileName,
      std::string phyStateTraceCSVFileName,
      std::string macPacketTraceCSVFileName,
      std::string macStateTraceCSVFileName,
      std::string edMsgTraceCSVFileName,
      std::string nsDSMsgTraceCSVFileName,
      std::string miscTraceCSVFileName,
      std::string nodesCSVFileName)
{
  m_loraEDNodeContainer = loraEDNodeContainer;
  m_loraGWNodeContainer = loraGWNodeContainer;
  m_loraEDDevices = loraEDDevices;
  m_loraGWDevices = loraGWDevices;

  m_nLoraEndDevices = nLoraDevices;
  m_nLoraGateways = nLoraGateways;

  m_discRadius = discRadius;
  m_totalTime = totalTime;

  m_usPacketSize = usPacketSize;
  m_usMaxBytes = usMaxBytes;
  m_usUnconfirmedDataNbRep = usUnconfirmedDataNbRep;
  m_usConfirmdData = usConfirmedData;

  m_dsDataGenerate = dsDataGenerate;
  m_dsPacketSize = dsPacketSize;
  m_dsDataExpMean = dsDataExpMean;
  m_dsConfirmedData = dsConfirmedData;

  m_phyTransmissionTraceCSVFileName = phyTransmissionTraceCSVFileName;
  m_phyStateTraceCSVFileName = phyStateTraceCSVFileName;
  m_macPacketTraceCSVFileName = macPacketTraceCSVFileName;
  m_macStateTraceCSVFileName = macStateTraceCSVFileName;
  m_edMsgTraceCSVFileName = edMsgTraceCSVFileName;
  m_nsDSMsgTraceCSVFileName = nsDSMsgTraceCSVFileName;

  m_miscTraceCSVFileName = miscTraceCSVFileName;
  m_nodesCSVFileName = nodesCSVFileName;

  SetupTracing (tracePhyTransmissions, tracePhyStates, traceMacPackets, traceMacStates, traceEdMsgs, traceNsDsMsgs, traceMisc);
  OutputNodesToFile ();
}

std::ostringstream
LoRaWANExampleTracing::GeneratePhyTraceOutputLine (
    std::string traceSource, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet, 
    bool insertNewLine)
{
  // Generate hex string of packet
  std::ostringstream packetHex;
  packetHex << std::hex << std::setfill('0');
  uint8_t bufferSize = packet->GetSize ();
  uint8_t buffer[bufferSize];
  uint32_t nCopied = packet->CopyData (buffer, bufferSize);
  for (uint32_t i=0; i<nCopied; i++) {
    packetHex << std::setw(2) << static_cast<unsigned>(buffer[i]);
    //if ((i+1) % 4 == 0)
    //  packetHex << " ";
  }

  // Get Trace Tag added by Phy
  LoRaWANPhyTraceIdTag traceTag;
  std::string traceTagOutput = "-1";
  if (packet->PeekPacketTag (traceTag)) {
    traceTagOutput = std::to_string (traceTag.GetFlowId ());
  }

  // Generate output line
  std::ostringstream output;
  output << std::setiosflags (std::ios::fixed) 
    << std::setprecision (9) 
    << Simulator::Now ().GetSeconds () << ","
    << device->GetDeviceType () << ","
    << device->GetNode ()->GetId () << ","
    << device->GetIfIndex () << ","
    << static_cast<uint32_t>(phy->GetIndex ()) << ","
    << traceSource << ","
    << traceTagOutput << ","
    << packetHex.str () << ","
    << packet->GetSize ();

  if (insertNewLine)
    output << std::endl;

  return output;
}

void
LoRaWANExampleTracing::PhyTxBegin (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GeneratePhyTraceOutputLine ("PhyTxBegin", device, phy, packet, false);

  // add channel and data rate index of Phy to output
  uint8_t channelIndex = phy->GetCurrentChannelIndex ();
  uint8_t dataRateIndex = phy->GetCurrentDataRateIndex ();;
  output << "," << (uint32_t)channelIndex << "," << (uint32_t)dataRateIndex <<  std::endl;
  example->LogOutputLine (output.str (), example->m_phyTransmissionTraceCSVFileName);
}

void
LoRaWANExampleTracing::PhyTxEnd (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GeneratePhyTraceOutputLine ("PhyTxEnd", device, phy, packet);

  example->LogOutputLine (output.str (), example->m_phyTransmissionTraceCSVFileName);
}

void
LoRaWANExampleTracing::PhyTxDrop (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GeneratePhyTraceOutputLine ("PhyTxDrop", device, phy, packet);

  example->LogOutputLine (output.str (), example->m_phyTransmissionTraceCSVFileName);
}

void
LoRaWANExampleTracing::PhyRxBegin (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GeneratePhyTraceOutputLine ("PhyRxBegin", device, phy, packet);

  example->LogOutputLine (output.str (), example->m_phyTransmissionTraceCSVFileName);
}

void
LoRaWANExampleTracing::PhyRxEnd (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet, double lqi)
{
  std::ostringstream output = 
    example->GeneratePhyTraceOutputLine ("PhyRxEnd", device, phy, packet, false);

  output << "," << lqi << std::endl;
  example->LogOutputLine (output.str (), example->m_phyTransmissionTraceCSVFileName);
}

void
LoRaWANExampleTracing::PhyRxDrop (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    Ptr<const Packet> packet, 
    LoRaWANPhyDropRxReason dropReason)
{
  std::ostringstream output = 
    example->GeneratePhyTraceOutputLine ("PhyRxDrop", device, phy, packet, false);

  output << "," << dropReason << std::endl;
  example->LogOutputLine (output.str (), example->m_phyTransmissionTraceCSVFileName);
}


std::ostringstream
LoRaWANExampleTracing::GenerateMacTraceOutputLine (
    std::string traceSource, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet, 
    bool insertNewLine)
{
  // Generate hex string of packet
  std::ostringstream packetHex;
  packetHex << std::hex << std::setfill('0');
  uint8_t bufferSize = packet->GetSize ();
  uint8_t buffer[bufferSize];
  uint32_t nCopied = packet->CopyData (buffer, bufferSize);
  for (uint32_t i=0; i<nCopied; i++) {
    packetHex << std::setw(2) << static_cast<unsigned>(buffer[i]);
  }

  // Generate output line
  std::ostringstream output;
  output << std::setiosflags (std::ios::fixed) 
    << std::setprecision (9) 
    << Simulator::Now ().GetSeconds () << ","
    << device->GetDeviceType () << ","
    << device->GetNode ()->GetId () << ","
    << device->GetIfIndex () << ","
    << static_cast<uint32_t>(mac->GetIndex ()) << ","
    << traceSource << ","
    << packetHex.str () << ","
    << packet->GetSize ();

  if (insertNewLine)
    output << std::endl;

  return output;
}

void
LoRaWANExampleTracing::MacTx (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = example->GenerateMacTraceOutputLine ("MacTx", device, mac, packet);
  example->LogOutputLine (output.str (), example->m_macPacketTraceCSVFileName);
}

void
LoRaWANExampleTracing::MacTxOk (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = example->GenerateMacTraceOutputLine ("MacTxOk", device, mac, packet);
  example->LogOutputLine (output.str (), example->m_macPacketTraceCSVFileName);
}

void
LoRaWANExampleTracing::MacTxDrop (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GenerateMacTraceOutputLine ("MacTxDrop", device, mac, packet);

  example->LogOutputLine (output.str (), example->m_macPacketTraceCSVFileName);
}

void
LoRaWANExampleTracing::MacRx (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = example->GenerateMacTraceOutputLine ("MacRx", device, mac, packet);
  example->LogOutputLine (output.str (), example->m_macPacketTraceCSVFileName);
}

void
LoRaWANExampleTracing::MacRxDrop (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GenerateMacTraceOutputLine ("MacRxDrop", device, mac, packet);

  example->LogOutputLine (output.str (), example->m_macPacketTraceCSVFileName);
}

void
LoRaWANExampleTracing::MacSentPkt (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    Ptr<const Packet> packet, 
    uint8_t n_transmissions)
{
  std::ostringstream output = 
    example->GenerateMacTraceOutputLine ("MacSentPkt", device, mac, packet, false);

  output << "," << (uint32_t)n_transmissions << std::endl;
  example->LogOutputLine (output.str (), example->m_macPacketTraceCSVFileName);
}

void
LoRaWANExampleTracing::PhyStateChangeNotification (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANPhy> phy, 
    LoRaWANPhyEnumeration oldState, 
    LoRaWANPhyEnumeration newState)
{
  std::ostringstream output;
  output << std::setiosflags (std::ios::fixed) 
    << std::setprecision (9) 
    << Simulator::Now ().GetSeconds () << ","
    << device->GetDeviceType () << ","
    << device->GetNode ()->GetId () << ","
    << device->GetIfIndex () << ","
    << static_cast<uint32_t>(phy->GetIndex ()) << ","
    << "PhyState" << ","
    << oldState << ","
    << newState
    << std::endl;

  example->LogOutputLine (output.str (), example->m_phyStateTraceCSVFileName);
}

void
LoRaWANExampleTracing::MacStateChangeNotification (
    LoRaWANExampleTracing* example, 
    Ptr<LoRaWANNetDevice> device, 
    Ptr<LoRaWANMac> mac, 
    LoRaWANMacState oldState, 
    LoRaWANMacState newState)
{
  std::ostringstream output;
  output << std::setiosflags (std::ios::fixed) 
    << std::setprecision (9) 
    << Simulator::Now ().GetSeconds () << ","
    << device->GetDeviceType () << ","
    << device->GetNode ()->GetId () << ","
    << device->GetIfIndex () << ","
    << static_cast<uint32_t>(mac->GetIndex ()) << ","
    << "MacState" << ","
    << oldState << ","
    << newState
    << std::endl;

  example->LogOutputLine (output.str (), example->m_macStateTraceCSVFileName);
}

void
LoRaWANExampleTracing::nrRW1SentTrace (
    LoRaWANExampleTracing* example, 
    uint32_t oldValue, 
    uint32_t newValue)
{
  example->m_nrRW1Sent = newValue;
}

void
LoRaWANExampleTracing::nrRW2SentTrace (
    LoRaWANExampleTracing* example, 
    uint32_t oldValue, 
    uint32_t newValue)
{
  example->m_nrRW2Sent = newValue;
}
void
LoRaWANExampleTracing::nrRW1MissedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t oldValue, 
    uint32_t newValue)
{
  example->m_nrRW1Missed = newValue;
}

void
LoRaWANExampleTracing::nrRW2MissedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t oldValue, 
    uint32_t newValue)
{
  example->m_nrRW2Missed = newValue;
}

std::ostringstream
LoRaWANExampleTracing::GenerateEDMsgTraceOutputLine (
    std::string traceSource, 
    uint32_t deviceAddress, 
    uint8_t msgType, 
    Ptr<const Packet> packet, 
    bool insertNewLine)
{
  // Generate hex string of packet
  std::ostringstream packetHex;
  packetHex << std::hex << std::setfill('0');

  uint8_t bufferSize = packet->GetSize ();
  uint8_t buffer[bufferSize];
  uint32_t nCopied = packet->CopyData (buffer, bufferSize);
  for (uint32_t i=0; i<nCopied; i++) {
    packetHex << std::setw(2) << static_cast<unsigned>(buffer[i]);
  }

  // Generate output line
  std::ostringstream output;
  output << std::setiosflags (std::ios::fixed) 
    << std::setprecision (9) 
    << Simulator::Now ().GetSeconds () << ","
    << traceSource << ","
    << deviceAddress << ","
    << (uint32_t) msgType << ","
    << packetHex.str () << ","
    << packet->GetSize ();

  if (insertNewLine)
    output << std::endl;

  return output;
}

void
LoRaWANExampleTracing::USMsgTransmittedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t msgType, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GenerateEDMsgTraceOutputLine ("USMsgTx", deviceAddress, msgType, packet);

  example->LogOutputLine (output.str (), example->m_edMsgTraceCSVFileName);
}

void
LoRaWANExampleTracing::DSMsgReceivedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t msgType, 
    Ptr<const Packet> packet, 
    uint8_t rw)
{
  std::ostringstream output = 
    example->GenerateEDMsgTraceOutputLine ("DSMsgRx", deviceAddress, msgType, packet, false);

  // add receive window to output
  output << "," << (unsigned)rw << std::endl;
  example->LogOutputLine (output.str (), example->m_edMsgTraceCSVFileName);
}

std::ostringstream
LoRaWANExampleTracing::GenerateNSDSMsgTraceOutputLine (
    std::string traceSource, 
    uint32_t deviceAddress, 
    uint8_t transmissionsRemaning, 
    uint8_t msgType, 
    Ptr<const Packet> packet, 
    bool insertNewLine)
{
  // Generate hex string of packet
  std::ostringstream packetHex;
  packetHex << std::hex << std::setfill('0');

  uint8_t bufferSize = packet->GetSize ();
  uint8_t buffer[bufferSize];
  uint32_t nCopied = packet->CopyData (buffer, bufferSize);
  for (uint32_t i=0; i<nCopied; i++) {
    packetHex << std::setw(2) << static_cast<unsigned>(buffer[i]);
  }

  // Generate output line
  std::ostringstream output;
  output << std::setiosflags (std::ios::fixed) << std::setprecision (9) << Simulator::Now ().GetSeconds () << ","
    << traceSource << ","
    << deviceAddress << ","
    << (uint32_t) msgType << ","
    << (uint32_t) transmissionsRemaning << ","
    << packetHex.str () << ","
    << packet->GetSize ();

  if (insertNewLine)
    output << std::endl;

  return output;
}

void
LoRaWANExampleTracing::DSMsgTransmittedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t transmissionsRemaning, 
    uint8_t msgType, 
    Ptr<const Packet> packet, 
    uint8_t rw)
{
  std::ostringstream output = example->GenerateNSDSMsgTraceOutputLine ("DSMsgTx", deviceAddress, transmissionsRemaning, msgType, packet, false);
  // add receive window to output
  output << "," << (unsigned)rw << std::endl;
  example->LogOutputLine (output.str (), example->m_nsDSMsgTraceCSVFileName);
}

void
LoRaWANExampleTracing::DSMsgGeneratedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t transmissionsRemaning, 
    uint8_t msgType, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GenerateNSDSMsgTraceOutputLine ("DSMsgGenerated", deviceAddress, transmissionsRemaning, msgType, packet);

  example->LogOutputLine (output.str (), example->m_nsDSMsgTraceCSVFileName);
}

void
LoRaWANExampleTracing::DSMsgAckdTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t transmissionsRemaning, 
    uint8_t msgType, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = example->GenerateNSDSMsgTraceOutputLine 
    ("DSMsgAckd", deviceAddress, transmissionsRemaning, msgType, packet);

  example->LogOutputLine (output.str (), example->m_nsDSMsgTraceCSVFileName);
}

void
LoRaWANExampleTracing::DSMsgDroppedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t transmissionsRemaning, 
    uint8_t msgType, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = example->GenerateNSDSMsgTraceOutputLine 
    ("DSMsgDrop", deviceAddress, transmissionsRemaning, msgType, packet);

  example->LogOutputLine (output.str (), example->m_nsDSMsgTraceCSVFileName);
}

void
LoRaWANExampleTracing::USMsgReceivedTrace (
    LoRaWANExampleTracing* example, 
    uint32_t deviceAddress, 
    uint8_t msgType, 
    Ptr<const Packet> packet)
{
  std::ostringstream output = 
    example->GenerateNSDSMsgTraceOutputLine ("USMsgRx", deviceAddress, 0, msgType, packet);

  example->LogOutputLine (output.str (), example->m_nsDSMsgTraceCSVFileName);
}

void
LoRaWANExampleTracing::WriteMiscStatsToFile ()
{
  std::ofstream out (m_miscTraceCSVFileName.c_str (), std::ios::app);
  out << m_nrRW1Sent << ","
    << m_nrRW2Sent << ","
    << m_nrRW1Missed << ","
    << m_nrRW2Missed
    << std::endl;
  out.close ();
}

void
LoRaWANExampleTracing::SetupTracing (
    bool tracePhyTransmissions, 
    bool tracePhyStates, 
    bool traceMacPackets,
    bool traceMacStates, 
    bool traceEdMsgs, 
    bool traceNsDsMsgs, 
    bool traceMisc)
{
  // Connect trace sources.
  // Trace Phy transmissions:
  if (tracePhyTransmissions) {
    for (NodeContainer::Iterator i = m_loraEDNodeContainer.Begin (); 
        i != m_loraEDNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      Ptr<LoRaWANPhy> phy = netDevice->GetPhy ();
      phy->TraceConnectWithoutContext ("PhyTxBegin", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyTxBegin, this, netDevice, phy));
      phy->TraceConnectWithoutContext ("PhyTxEnd", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyTxEnd, this, netDevice, phy));
      phy->TraceConnectWithoutContext ("PhyTxDrop", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyTxDrop, this, netDevice, phy));
      phy->TraceConnectWithoutContext ("PhyRxBegin", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyRxBegin, this, netDevice, phy));
      phy->TraceConnectWithoutContext ("PhyRxEnd", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyRxEnd, this, netDevice, phy));
      phy->TraceConnectWithoutContext ("PhyRxDrop", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyRxDrop, this, netDevice, phy));
    }

    for (NodeContainer::Iterator i = m_loraGWNodeContainer.Begin (); 
        i != m_loraGWNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      for (auto &phy : netDevice->GetPhys() ) {
        phy->TraceConnectWithoutContext ("PhyTxBegin", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyTxBegin, this, netDevice, phy));
        phy->TraceConnectWithoutContext ("PhyTxEnd", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyTxEnd, this, netDevice, phy));
        phy->TraceConnectWithoutContext ("PhyTxDrop", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyTxDrop, this, netDevice, phy));
        phy->TraceConnectWithoutContext ("PhyRxBegin", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyRxBegin, this, netDevice, phy));
        phy->TraceConnectWithoutContext ("PhyRxEnd", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyRxEnd, this, netDevice, phy));
        phy->TraceConnectWithoutContext ("PhyRxDrop", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyRxDrop, this, netDevice, phy));
      }
    }
  }

  if (tracePhyStates) {
    // Trace state changes in the phy and mac
    for (NodeContainer::Iterator i = m_loraEDNodeContainer.Begin (); 
        i != m_loraEDNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      Ptr<LoRaWANPhy> phy = netDevice->GetPhy ();
      phy->TraceConnectWithoutContext ("TrxState", 
          MakeBoundCallback (&LoRaWANExampleTracing::PhyStateChangeNotification, 
            this, netDevice, phy));
    }

    for (NodeContainer::Iterator i = m_loraGWNodeContainer.Begin (); 
        i != m_loraGWNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      for (auto &phy : netDevice->GetPhys ()) {
        phy->TraceConnectWithoutContext ("TrxState", 
            MakeBoundCallback (&LoRaWANExampleTracing::PhyStateChangeNotification, 
              this, netDevice, phy));
      }
    }
  }

  if (traceMacPackets) {
    for (NodeContainer::Iterator i = m_loraEDNodeContainer.Begin (); 
        i != m_loraEDNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      Ptr<LoRaWANMac> mac = netDevice->GetMac ();

      mac->TraceConnectWithoutContext ("MacTx", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacTx, this, netDevice, mac));
      mac->TraceConnectWithoutContext ("MacTxOk", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacTxOk, this, netDevice, mac));
      mac->TraceConnectWithoutContext ("MacTxDrop", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacTxDrop, this, netDevice, mac));
      mac->TraceConnectWithoutContext ("MacRx", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacRx, this, netDevice, mac));
      mac->TraceConnectWithoutContext ("MacRxDrop", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacRxDrop, this, netDevice, mac));
      mac->TraceConnectWithoutContext ("MacSentPkt", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacSentPkt, this, netDevice, mac));
    }

    for (NodeContainer::Iterator i = m_loraGWNodeContainer.Begin (); 
        i != m_loraGWNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      for (auto &mac : netDevice->GetMacs ()) {
        mac->TraceConnectWithoutContext ("MacTx", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacTx, this, netDevice, mac));
        mac->TraceConnectWithoutContext ("MacTxOk", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacTxOk, this, netDevice, mac));
        mac->TraceConnectWithoutContext ("MacTxDrop", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacTxDrop, this, netDevice, mac));
        mac->TraceConnectWithoutContext ("MacRx", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacRx, this, netDevice, mac));
        mac->TraceConnectWithoutContext ("MacRxDrop", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacRxDrop, this, netDevice, mac));
        mac->TraceConnectWithoutContext ("MacSentPkt", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacSentPkt, this, netDevice, mac));
      }
    }

  }

  if (traceMacStates) {
    // Trace state changes in the phy and mac
    for (NodeContainer::Iterator i = m_loraEDNodeContainer.Begin (); 
        i != m_loraEDNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      Ptr<LoRaWANMac> mac = netDevice->GetMac ();
      mac->TraceConnectWithoutContext ("MacState", 
          MakeBoundCallback (&LoRaWANExampleTracing::MacStateChangeNotification, 
          this, netDevice, mac));
    }

    for (NodeContainer::Iterator i = m_loraGWNodeContainer.Begin (); 
        i != m_loraGWNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
      for (auto &mac : netDevice->GetMacs ()) {
        mac->TraceConnectWithoutContext ("MacState", 
            MakeBoundCallback (&LoRaWANExampleTracing::MacStateChangeNotification, 
              this, netDevice, mac));
      }
    }

  }

  if (traceEdMsgs) {
    for (NodeContainer::Iterator i = m_loraEDNodeContainer.Begin (); 
        i != m_loraEDNodeContainer.End (); ++i)
    {
      Ptr<Node> node = *i;
      NS_ASSERT (node != 0);
      Ptr<LoRaWANEndDeviceApplication> edApp = 
        node->GetApplication (0)->GetObject<LoRaWANEndDeviceApplication> ();

      edApp->TraceConnectWithoutContext ("USMsgTransmitted", 
          MakeBoundCallback (&LoRaWANExampleTracing::USMsgTransmittedTrace, this));

      edApp->TraceConnectWithoutContext ("DSMsgReceived", 
          MakeBoundCallback (&LoRaWANExampleTracing::DSMsgReceivedTrace, this));
    }
  }

  if (traceNsDsMsgs) {
    Ptr<LoRaWANNetworkServer> lorawanNSPtr = 
      LoRaWANNetworkServer::getLoRaWANNetworkServerPointer ();

    NS_ASSERT (lorawanNSPtr);
    if (lorawanNSPtr) {
      lorawanNSPtr->TraceConnectWithoutContext ("DSMsgGenerated", 
          MakeBoundCallback (&LoRaWANExampleTracing::DSMsgGeneratedTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("DSMsgTransmitted", 
          MakeBoundCallback (&LoRaWANExampleTracing::DSMsgTransmittedTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("DSMsgAckd", 
          MakeBoundCallback (&LoRaWANExampleTracing::DSMsgAckdTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("DSMsgDropped", 
          MakeBoundCallback (&LoRaWANExampleTracing::DSMsgDroppedTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("USMsgReceived", 
          MakeBoundCallback (&LoRaWANExampleTracing::USMsgReceivedTrace, this));
    }
  }

  if (traceMisc) {
    Ptr<LoRaWANNetworkServer> lorawanNSPtr = 
      LoRaWANNetworkServer::getLoRaWANNetworkServerPointer ();

    NS_ASSERT (lorawanNSPtr);

    if (lorawanNSPtr) {
      lorawanNSPtr->TraceConnectWithoutContext ("nrRW1Sent", 
          MakeBoundCallback (&LoRaWANExampleTracing::nrRW1SentTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("nrRW2Sent", 
          MakeBoundCallback (&LoRaWANExampleTracing::nrRW2SentTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("nrRW1Missed", 
          MakeBoundCallback (&LoRaWANExampleTracing::nrRW1MissedTrace, this));
      lorawanNSPtr->TraceConnectWithoutContext ("nrRW2Missed", 
          MakeBoundCallback (&LoRaWANExampleTracing::nrRW2MissedTrace, this));
    }
  }
}

void
LoRaWANExampleTracing::OutputNodesToFile ()
{
  std::ofstream outNodes (m_nodesCSVFileName.c_str (), std::ios::app);
  // Output For Gateways: nodeid, device type, x, y
  for (NodeContainer::Iterator i = m_loraGWNodeContainer.Begin (); 
      i != m_loraGWNodeContainer.End (); ++i)
  {
    Ptr<Node> node = *i;
    Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));
    Ptr<MobilityModel> gatewayMobility = node->GetObject<MobilityModel> ();
    NS_ASSERT (gatewayMobility);
    Vector gatewayPos = gatewayMobility->GetPosition ();

    outNodes
      << node->GetId () << ","
      << netDevice->GetDeviceType () << ","
      << gatewayPos.x << ","
      << gatewayPos.y << std::endl;
  }

  // Output For End Devices: nodeid, device type, x, y, distancetoclosestgateway, datarateindex
  for (NodeContainer::Iterator i = m_loraEDNodeContainer.Begin (); 
      i != m_loraEDNodeContainer.End (); ++i)
  {
    Ptr<Node> node = *i;
    Ptr<LoRaWANNetDevice> netDevice = DynamicCast<LoRaWANNetDevice> (node->GetDevice (0));

    Ptr<MobilityModel> endDeviceMobility = node->GetObject<MobilityModel> ();
    NS_ASSERT (endDeviceMobility);
    Vector endDevicePos = endDeviceMobility->GetPosition ();

    // find distance to the closest gateway:
    Vector closestGwPos; // to be determined
    double dmin = std::numeric_limits<double>::max ();
    {
    for (NodeContainer::Iterator it_gw = m_loraGWNodeContainer.Begin (); 
        it_gw != m_loraGWNodeContainer.End (); ++it_gw)
    {
      Ptr<MobilityModel> gatewayMobility = (*it_gw)->GetObject<MobilityModel> ();
      NS_ASSERT (gatewayMobility);
      Vector gwPos = gatewayMobility->GetPosition ();

      double distance = CalculateDistance (endDevicePos, gwPos);
      if (distance < dmin) {
        dmin = distance;
        closestGwPos = gwPos;
      }
    }
    }

    Ptr<LoRaWANEndDeviceApplication> edApp = 
      node->GetApplication (0)->GetObject<LoRaWANEndDeviceApplication> ();

    uint8_t dataRateIndex = edApp->GetDataRateIndex ();

    outNodes
      << node->GetId () << ","
      << netDevice->GetDeviceType () << ","
      << endDevicePos.x << ","
      << endDevicePos.y << ","
      << dmin << ","
      << (uint32_t)dataRateIndex << std::endl;
  }
  outNodes.close ();
}

void
LoRaWANExampleTracing::LogOutputLine (std::string output, std::string fileName)
{
  // creates the stream object and returns reference
  std::ofstream & outputstream = output_streams[fileName];   

  if (!outputstream.is_open()) {
    outputstream.open(fileName.c_str(), std::ios::app);
  }
  if (outputstream.is_open()) {
    outputstream << output << std::endl;
  }
}

