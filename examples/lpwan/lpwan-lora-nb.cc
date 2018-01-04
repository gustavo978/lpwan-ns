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

typedef enum {
  TestLoraNbDifferentBs = 1,
  TestLoraNbSameBs = 2,
  TestLoraNbCoordenate = 3,
} TestType;

typedef enum {
  LORAWAN_DR_CALC_METHOD_PER_INDEX = 0x00,
  LORAWAN_DR_CALC_METHOD_RANDOM_INDEX = 0x01,
  LORAWAN_DR_CALC_METHOD_FIXED_INDEX = 0x02,
} LoRaWANDataRateCalcMethodIndex;

class LpwanTracing {
  public:
    void CaseRun (uint32_t nLoraDevices,
        uint32_t nNbDevices,
        uint32_t nLoraGateways,
        uint32_t nNbGateways,
        double discRadius,
        double totalTime,
        uint32_t usPacketSize,
        uint32_t usMaxBytes,
        double usDataPeriod,
        uint32_t usUnconfirmedDataNbRep,
        bool usConfirmedData,
        uint32_t dsPacketSize,
        bool dsDataGenerate,
        double dsDataExpMean,
        bool dsConfirmedData,
        bool verbose);

    // 1B MAC header, 8B frame header, 8 byte payload and 4B MIC
    constexpr static const uint8_t m_perPacketSize = 1 + 8 + 8 + 4; 
    uint8_t CalculateDataRateIndexPER (Ptr<Application> endDeviceApp);
    uint8_t CalculateRandomDataRateIndex (Ptr<Application> endDeviceApp);
    uint8_t CalculateFixedDataRateIndex (Ptr<Application> endDeviceApp);
    void SelectDRCalculationMethod (LoRaWANDataRateCalcMethodIndex loRaWANDataRateCalcMethodIndex);
    void SetDRCalcPerLimit (double drCalcPerLimit);
    void SetDrCalcFixedDrIndex (uint8_t fixedDataRateIndex);

    void TestNb();
  private:
    uint32_t m_nLoraDevices;
    uint32_t m_nLoraGateways;
    uint32_t m_nNbDevices;
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

    uint32_t m_nrRW1Sent;
    uint32_t m_nrRW2Sent;
    uint32_t m_nrRW1Missed;
    uint32_t m_nrRW2Missed;

    NodeContainer m_loraEndDeviceNodes;
    NodeContainer m_loraGatewayNodes;
    NodeContainer m_nbEndDeviceNodes;
    NodeContainer m_nbGatewayNodes;

    NodeContainer m_allNodes;

    NetDeviceContainer m_loraEDDevices;
    NetDeviceContainer m_loraGWDevices;
    NetDeviceContainer m_nbEDDevices;
    NetDeviceContainer m_nbGWDevices;

    Ptr<LteHelper> lteHelper;

  private:
    void LoraCreateNodes ();
    void NbCreateNodes ();

    void SetupMobility ();
    //void NbSetupMobility();

    void LoraCreateDevices ();
    void NbCreateDevices ();

    void LoraInstallApplications();
    void NbInstallApplications();

    //void SetupTracing (bool tracePhyTransmissions, bool tracePhyStates, bool traceMacPackets, bool traceMacStates, bool traceEdMsgs, bool traceNsDsMsgs, bool traceMisc);
};

int main (int argc, char *argv[]) {

  uint32_t randomSeed = 12345;
  uint32_t nLoraDevices = 1;
  uint32_t nNbDevices = 1;
  uint32_t nLoraGateways = 1;
  uint32_t nNbGateways = 1;
  double discRadius = 5000.0;
  double totalTime = 600.0;
  uint32_t nRuns = 1;
  uint32_t drCalcMethodIndex = 2;
  double drCalcPerLimit = 0.01;
  double drCalcFixedDRIndex = 0;
  uint32_t usPacketSize = 21;
  uint32_t usMaxBytes = 0;
  double usDataPeriod = 600.0;
  uint32_t usUnconfirmedDataNbRep = 1;
  bool usConfirmedData = false;
  uint32_t dsPacketSize = 21;
  bool dsDataGenerate = false;
  double dsDataExpMean = -1;
  bool dsConfirmedData = false;
  bool verbose = false;
  std::string outputFileNamePrefix = "output/LoRaWAN-example-tracing";

  CommandLine cmd;

  cmd.AddValue ("randomSeed", 
      "Random seed used in experiments[Default:12345]", 
      randomSeed);

  cmd.AddValue ("nLoraDevices", 
      "Number of LoRaWAN class A end devices nodes[Default:30]", 
      nLoraDevices);

  cmd.AddValue ("nNbDevices", 
      "Number of NB-IoR end devices nodes[Default:30]", 
      nNbDevices);

  cmd.AddValue ("nLoraGateways", 
      "Number of LoRaWAN gateways [Default:1]", 
      nLoraGateways);

  cmd.AddValue ("nNbGateways", 
      "Number of NB-IoT gateways [Default:1]", 
      nNbGateways);

  cmd.AddValue ("discRadius", 
      "The radius of the disc (in meters) in which end devices and gateways are placed[Default:5000.0]", 
      discRadius);

  cmd.AddValue ("totalTime", 
      "Simulation time for one run in Seconds[Default:100]", 
      totalTime);

  cmd.AddValue ("nRuns", 
      "Number of simulation runs[Default:100]", 
      nRuns);

  cmd.AddValue ("drCalcMethodIndex", 
      "Method to use for end device Data Rate calculation[Default:0]", 
      drCalcMethodIndex);

  cmd.AddValue ("drCalcPerLimit", 
      "PER limit to use when using the PER Data Rate Calculation method (use index = 0)[Default:0.01]", 
      drCalcPerLimit);

  cmd.AddValue ("drCalcFixedDRIndex", 
      "Fixed Data Rate index to assign when using the Fixed Data Rate Calculation method (use index = 2)[Default:0]", 
      drCalcFixedDRIndex);

  cmd.AddValue ("usPacketSize", 
      "Packet size used for generating US packets[Default:21]",
      usPacketSize);

  cmd.AddValue ("usMaxBytes", 
      "Maximum number of bytes to be queued for transmission in the upstream direction, note this does not take into account retransmissions. 0 means no limit. [Default:0]", 
      usMaxBytes);

  cmd.AddValue ("usDataPeriod", 
      "Period between subsequent Upstream data transmissions from an end device[Default:600]", 
      usDataPeriod);

  cmd.AddValue ("usUnconfirmedDataNbRep", 
      "Number of repetitions for unconfirmed US data messages[Default:1]", 
      usUnconfirmedDataNbRep);

  cmd.AddValue ("usConfirmedData", 
      "0 for Unconfirmed Upstream Data MAC packets, 1 for Confirmed Upstream Data MAC Packets[Default:0]", 
      usConfirmedData);

  cmd.AddValue ("dsPacketSize", 
      "Packet size used for generating DS packets[Default:21]", 
      dsPacketSize);

  cmd.AddValue ("dsDataGenerate", 
      "Should NS generate DS traffic for end devices, note that Acks are always sent.[Default:0]",
      dsDataGenerate);

  cmd.AddValue ("dsDataExpMean", 
      "Mean for the Exponential random variable for inter packet time for DS transmission for an end device[Default:10*usDataPeriod]", 
      dsDataExpMean);

  cmd.AddValue ("dsConfirmedData", 
      "0 for Unconfirmed Downstream Data MAC packets, 1 for Confirmed Downstream Data MAC Packets[Default:0]", 
      dsConfirmedData);

  cmd.AddValue ("verbose", 
      "turn on all log components[Default:0]", 
      verbose);

  cmd.AddValue ("outputFileNamePrefix", 
      "The prefix for the names of the output files[Default:output/LoRaWAN-example-tracing]", 
      outputFileNamePrefix);

  cmd.Parse (argc, argv);

  if (dsDataExpMean == -1) {
    dsDataExpMean = 10 * usDataPeriod; // set default value for dsDataExpMean
  }

  LoRaWANDataRateCalcMethodIndex loRaWANDataRateCalcMethodIndex;
  if (drCalcMethodIndex == 0) {
    loRaWANDataRateCalcMethodIndex = LORAWAN_DR_CALC_METHOD_PER_INDEX;
  } else if (drCalcMethodIndex == 1) {
    loRaWANDataRateCalcMethodIndex = LORAWAN_DR_CALC_METHOD_RANDOM_INDEX;
  } else if (drCalcMethodIndex == 2) {
    loRaWANDataRateCalcMethodIndex = LORAWAN_DR_CALC_METHOD_FIXED_INDEX;
  } else {
    std::cout << " Invalid Data Rate Calculcation Method index provided: " << (uint32_t)drCalcMethodIndex << std::endl;
    return -1;
  }

  //std::time_t unix_epoch = std::time(nullptr);
  for (uint32_t i = 0; i < nRuns; i++) {
    uint32_t seed = randomSeed + i;
    SeedManager::SetSeed (seed);

    LpwanTracing example = LpwanTracing ();
    example.SelectDRCalculationMethod (loRaWANDataRateCalcMethodIndex);
    example.SetDRCalcPerLimit (drCalcPerLimit);
    example.SetDrCalcFixedDrIndex (drCalcFixedDRIndex);
    example.CaseRun(nLoraDevices, nNbDevices, nLoraGateways, 
        nNbGateways, discRadius, totalTime,
        usPacketSize, usMaxBytes, usDataPeriod, 
        usUnconfirmedDataNbRep, usConfirmedData,
        dsPacketSize, dsDataGenerate, dsDataExpMean, dsConfirmedData,
        verbose);
  }

  return 0;
}


/*****************************************************
 * Public Functions
 * **************************************************/
void LpwanTracing::SelectDRCalculationMethod (
    LoRaWANDataRateCalcMethodIndex loRaWANDataRateCalcMethodIndex){

  if (loRaWANDataRateCalcMethodIndex == 0)
    this->m_drCalcMethodIndex = LORAWAN_DR_CALC_METHOD_PER_INDEX;
  else if (loRaWANDataRateCalcMethodIndex == 1)
    this->m_drCalcMethodIndex = LORAWAN_DR_CALC_METHOD_RANDOM_INDEX;
  else if (loRaWANDataRateCalcMethodIndex == 2)
    this->m_drCalcMethodIndex = LORAWAN_DR_CALC_METHOD_FIXED_INDEX;
  else
    std::cout << " Invalid Data Rate Calculcation Method index provided: " << (unsigned)loRaWANDataRateCalcMethodIndex << std::endl;
}

void LpwanTracing::SetDRCalcPerLimit (double drCalcPerLimit){
  NS_ASSERT (drCalcPerLimit >= 0.0 && drCalcPerLimit <= 1.0);
  if (drCalcPerLimit >= 0.0 && drCalcPerLimit <= 1.0) {
    this->m_drCalcPerLimit = drCalcPerLimit;
  } else {
    std::cout << " Invalid Data Rate PER Calculcation PER limit provided: " << drCalcPerLimit << std::endl;
  }
}

void LpwanTracing::SetDrCalcFixedDrIndex (uint8_t fixedDataRateIndex)
{
  if (fixedDataRateIndex < LoRaWAN::m_supportedDataRates.size() - 1)
    this->m_fixedDataRateIndex = fixedDataRateIndex;
  else {
    std::cout << "LpwanTracing::SetDrCalcFixedDrIndex: Invalid DR index provided: " << (uint32_t)fixedDataRateIndex << std::endl;
    exit(-1);
  }
}

void LpwanTracing::CaseRun( 
    uint32_t nLoraDevices,
    uint32_t nNbDevices,
    uint32_t nLoraGateways,
    uint32_t nNbGateways,
    double discRadius,
    double totalTime,
    uint32_t usPacketSize,
    uint32_t usMaxBytes,
    double usDataPeriod,
    uint32_t usUnconfirmedDataNbRep,
    bool usConfirmedData,
    uint32_t dsPacketSize,
    bool dsDataGenerate,
    double dsDataExpMean,
    bool dsConfirmedData,
    bool verbose)
{
  m_nLoraDevices = nLoraDevices;
  m_nLoraGateways = nLoraGateways;
  m_nNbDevices = nNbDevices;
  m_nNbGateways = nNbGateways;
  
  m_discRadius = discRadius;
  m_totalTime = totalTime;

  m_usPacketSize = usPacketSize;
  m_usMaxBytes = usMaxBytes;
  m_usDataPeriod = usDataPeriod;
  m_usUnconfirmedDataNbRep = usUnconfirmedDataNbRep;
  m_usConfirmdData = usConfirmedData;

  m_dsDataGenerate = dsDataGenerate;
  m_dsPacketSize = dsPacketSize;
  m_dsDataExpMean = dsDataExpMean;
  m_dsConfirmedData = dsConfirmedData;

  m_verbose = verbose;

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults ();

  //lteHelper->EnableTraces ();
  //lteHelper->EnableRlcTraces (); 

  // Create all nodes Gateways and Devices
  LoraCreateNodes ();
  NbCreateNodes ();

  // important: setup mobility before creating devices
  SetupMobility (); 

  // Create the end devices
  LoraCreateDevices ();
  NbCreateDevices ();

  LoraInstallApplications ();
  NbInstallApplications ();

  //SetupTracing (tracePhyTransmissions, tracePhyStates, traceMacPackets, traceMacStates, traceEdMsgs, traceNsDsMsgs, traceMisc);
  //OutputNodesToFile ();

  std::cout << "Starting simulation for " << m_totalTime << " s ...\n";

  Simulator::Stop (Seconds (m_totalTime));

  // netamin:
  //AnimationInterface anim ("lorawan-netamin.xml");
  //anim.EnablePacketMetadata (true);

  Simulator::Run ();

  Simulator::Destroy ();
  std::cout << "Fim\n";
}

/*****************************************************
 * Private Functions
 * **************************************************/
void LpwanTracing::LoraCreateNodes () {
  std::cout << "Creating " << (unsigned) m_nLoraGateways << " LoRaWAN Gateways.\n";
  std::cout << "Creating " << (unsigned) m_nLoraDevices << " LoRaWAN class A end devices.\n";
  m_loraGatewayNodes.Create(m_nLoraGateways);
  m_loraEndDeviceNodes.Create(m_nLoraDevices);
}

void LpwanTracing::NbCreateNodes () {
  std::cout << "Creating " << (unsigned) m_nNbDevices << " NB-IoT devices.\n";
  std::cout << "Creating " << (unsigned) m_nNbGateways << " NB-IoT Gateways.\n";
  m_nbGatewayNodes.Create(m_nNbGateways);
  m_nbEndDeviceNodes.Create(m_nNbDevices);
}

void LpwanTracing::SetupMobility () {
  MobilityHelper edMobility;
  edMobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
      "X", DoubleValue (0.0),
      "Y", DoubleValue (0.0),
      "rho", DoubleValue (m_discRadius));
  edMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  edMobility.Install (m_loraEndDeviceNodes);
  edMobility.Install (m_nbEndDeviceNodes);

  Ptr<ListPositionAllocator> nodePositionList = CreateObject<ListPositionAllocator> ();

  //Vector v1 = Vector(-m_discRadius/2 + 0*m_discRadius, 0.0, 0.0);
  Vector v1 = Vector(0.0, 0.0, 0.0);
  Vector v2 = Vector(-m_discRadius/2 + 1*m_discRadius, 0.0, 0.0);
  nodePositionList->Add (v1);
  nodePositionList->Add (v2);
  /*for (uint32_t i = 0; i < m_nLoraGateways; i++) {
    Vector v;
    if (m_nLoraGateways == 1) {
      v = Vector (0.0, 0.0, 0.0);
    } else if (m_nLoraGateways == 2) {
      v = Vector (-m_discRadius/2 + i*m_discRadius, 0.0, 0.0);
    } else if (m_nLoraGateways == 4) {
      v = Vector (-m_discRadius/2 + (i/2)*m_discRadius, -m_discRadius/2 + (i % 2)*m_discRadius, 0.0);
    } else {
      //NS_FATAL_ERROR (this << " " << m_nGateways << " number of gateways is not supported");
      exit (-1);
    }
    nodePositionList->Add (v);
  }*/

  MobilityHelper gwMobility;
  gwMobility.SetPositionAllocator (nodePositionList);
  gwMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  gwMobility.Install (m_loraGatewayNodes);
  gwMobility.Install (m_nbGatewayNodes);
}

/*void LpwanTracing::NbSetupMobility() {

  std::cout << "Set Mobility for NB-IoT devices\n";
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_nbGatewayNodes);
  BuildingsHelper::Install (m_nbGatewayNodes);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (m_nbEndDeviceNodes);
  BuildingsHelper::Install (m_nbEndDeviceNodes);
}*/

void LpwanTracing::NbCreateDevices () {
  lteHelper = CreateObject<LteHelper> ();
 
  m_nbGWDevices = lteHelper->InstallEnbDevice (m_nbGatewayNodes);
  m_nbEDDevices = lteHelper->InstallUeDevice (m_nbEndDeviceNodes);
}

void LpwanTracing::LoraCreateDevices () {
  LoRaWANHelper lorawanHelper;
  //lorawanHelper.EnableLogComponents (LOG_LEVEL_INFO);

  lorawanHelper.SetNbRep (m_usUnconfirmedDataNbRep);
  m_loraEDDevices = lorawanHelper.Install (m_loraEndDeviceNodes);
  lorawanHelper.SetDeviceType (LORAWAN_DT_GATEWAY);
  m_loraGWDevices = lorawanHelper.Install (m_loraGatewayNodes);
}

void LpwanTracing::LoraInstallApplications () {
  // PacketSocketHelper::Install adds a PacketSocketFactory 
  // object as an aggregate object to each of the nodes in the NodeContainer
  // This factory object is needed when applications create sockets in StartApplication
  PacketSocketHelper loraPacketSocket;
  loraPacketSocket.Install(m_loraEndDeviceNodes);
  loraPacketSocket.Install(m_loraGatewayNodes);

  // Gateways and Network Server:
  ApplicationContainer loraGatewayApps;
  ObjectFactory m_factory2; //!< Object factory.
  m_factory2.SetTypeId ("ns3::LoRaWANGatewayApplication");
  for (NodeContainer::Iterator i = m_loraGatewayNodes.Begin (); 
      i != m_loraGatewayNodes.End (); ++i) {
    Ptr<Node> node = *i;
    Ptr<Application> app = m_factory2.Create<Application> ();
    node->AddApplication (app);
    loraGatewayApps.Add (app);
    //Ptr<LoRaWANGatewayApplication> gwApp = DynamicCast<LoRaWANGatewayApplication> (app);
  }

  loraGatewayApps.Start (Seconds (0.0));
  loraGatewayApps.Stop (Seconds (m_totalTime));

  // Set attributes on LoRaWANNetworkServer object:
  Ptr<LoRaWANNetworkServer> lorawanNSPtr = LoRaWANNetworkServer::getLoRaWANNetworkServerPointer ();
  NS_ASSERT (lorawanNSPtr);
  if (lorawanNSPtr) {
    lorawanNSPtr->SetAttribute ("PacketSize", UintegerValue (m_dsPacketSize));
    lorawanNSPtr->SetAttribute ("GenerateDataDown", BooleanValue (m_dsDataGenerate));
    lorawanNSPtr->SetAttribute ("ConfirmedDataDown", BooleanValue (m_dsConfirmedData));
    std::stringstream downstreamiatss;
    downstreamiatss << "ns3::ExponentialRandomVariable[Mean=" << m_dsDataExpMean << "]";
    lorawanNSPtr->SetAttribute ("DownstreamIAT", StringValue (downstreamiatss.str ()));
  }

  // End devices:
  ApplicationContainer loraEndDeviceApp;
  ObjectFactory m_factory; //!< Object factory.
  m_factory.SetTypeId ("ns3::LoRaWANEndDeviceApplication");
  // Attributes shared by all end device apps:
  m_factory.Set ("PacketSize", UintegerValue (m_usPacketSize));
  m_factory.Set ("MaxBytes", UintegerValue (m_usMaxBytes));
  m_factory.Set ("ConfirmedDataUp", BooleanValue (m_usConfirmdData));
  std::stringstream upstreamiatss;
  upstreamiatss << "ns3::ConstantRandomVariable[Constant=" << m_usDataPeriod << "]";
  m_factory.Set ("UpstreamIAT", StringValue (upstreamiatss.str ()));
  // Limit the number of channels
  m_factory.Set ("ChannelRandomVariable", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

  Ptr<UniformRandomVariable> appStartRandomVariable = CreateObject<UniformRandomVariable> ();
  for (NodeContainer::Iterator i = m_loraEndDeviceNodes.Begin (); 
      i != m_loraEndDeviceNodes.End (); ++i) {
    Ptr<Node> node = *i;
    Ptr<Application> app = m_factory.Create<Application> ();
    node->AddApplication (app);

    // Start the end device apps at different times:
    double appStartRandomValue = appStartRandomVariable->GetValue (0, m_usDataPeriod);
    app->SetStartTime (Seconds (appStartRandomValue));
    app->SetStopTime(Seconds (m_totalTime));

    loraEndDeviceApp.Add (app);
  }

  // Assign data rate indexes to end device applications:
  for (ApplicationContainer::Iterator aci = loraEndDeviceApp.Begin (); 
      aci != loraEndDeviceApp.End (); ++aci) {
    Ptr<Application> app = *aci;
    uint8_t dataRateIndex = 0;
    if (m_drCalcMethodIndex == LORAWAN_DR_CALC_METHOD_PER_INDEX)
      dataRateIndex = CalculateDataRateIndexPER (app); // assign data rates based on PER vs distance
    else if (m_drCalcMethodIndex == LORAWAN_DR_CALC_METHOD_RANDOM_INDEX)
      dataRateIndex = CalculateRandomDataRateIndex(app); // assign random data rates
    else if (m_drCalcMethodIndex == LORAWAN_DR_CALC_METHOD_FIXED_INDEX)
      dataRateIndex = CalculateFixedDataRateIndex (app); // use a fixed data rate

    app->SetAttribute ("DataRateIndex", UintegerValue(dataRateIndex));
  }
}

uint8_t LpwanTracing::CalculateDataRateIndexPER (Ptr<Application> endDeviceApp) {
  // here we calculate the data rate index based on the distance between the
  // end device and the nearest gateway (the closer the node, the lower the
  // distance, the lower the path loss,  the higher the SINR, which allows for
  // a higher data rate)
  // we assign data rates so that the Packet Error Rate is below perLimit for a
  // perPacketSize packet sent at the position of the end device to the closest gateway
  const Ptr<Node> endDeviceNode = endDeviceApp->GetNode ();

  // !!!Important!!! we assume that the experiment is using only the LogDistancePropagationLossModel here
  Ptr<PropagationLossModel> lossModel = CreateObject<LogDistancePropagationLossModel> (); // TODO: can we get the loss model from the end device node object ?
  Ptr<MobilityModel> endDeviceMobility = endDeviceNode->GetObject<MobilityModel> ();
  //NS_ASSERT (endDeviceMobility);

  // find lowest path loss (or highest rx power) for all gateways
  double maxRxPowerdBm = std::numeric_limits<double>::lowest (); // note: numeric_limits::min() returns minimum positive normalized value for floating point types
  Vector posGw;
  for (NodeContainer::Iterator it = m_loraGatewayNodes.Begin (); 
      it != m_loraGatewayNodes.End (); ++it) {
    Ptr<MobilityModel> gatewayMobility = (*it)->GetObject<MobilityModel> ();
    //NS_ASSERT (gatewayMobility);

    const double rxPowerdBm = lossModel->CalcRxPower (14.0, endDeviceMobility, gatewayMobility); // 14.0 dBm TX power as this is max power for most EU868 sub bands

    if (rxPowerdBm > maxRxPowerdBm) {
      maxRxPowerdBm = rxPowerdBm;
      posGw = gatewayMobility->GetPosition ();
    }
  }

  LoRaWANSpectrumValueHelper psdHelper;
  const uint32_t freq = LoRaWAN::m_supportedChannels [0].m_fc;
  Ptr<const SpectrumValue> noise = psdHelper.CreateNoisePowerSpectralDensity (freq);

  double maxRxPowerLinear = pow (10.0, maxRxPowerdBm / 10.0) / 1000.0; // in Watts
  const double noisePowerLinear = LoRaWANSpectrumValueHelper::TotalAvgPower (noise, freq); // in Watts
  double snrDb = 10.0 * log10 (maxRxPowerLinear/noisePowerLinear);

  // For minimum path loss, search data rate so that PER is <= perLimit for a perPacketSize length packet
  // go from highest to lowest data rate, when PER drops below perLimit then we have found the data rate for the end device
  Ptr <LoRaWANErrorModel> errorModel = CreateObject<LoRaWANErrorModel> ();
  uint32_t nbits = 8*m_perPacketSize;
  uint8_t codeRate = 3;
  uint8_t calculatedDataRateIndex = LoRaWAN::m_supportedDataRates[0].dataRateIndex; // SF12 will be default SF

  //bool hitPerLimit = false; // check whether we actually get a per below the perLimit
  double perForDR = 0.0;
  auto it = LoRaWAN::m_supportedDataRates.rbegin(); // iterate in reverse, so that we start at SF7 and move upto SF12
  it++; // skip the 250kHz data rate
  for (; it != LoRaWAN::m_supportedDataRates.rend (); it++) {
    uint32_t bandWidth = it->bandWith;
    LoRaSpreadingFactor sf = it->spreadingFactor;
    perForDR = 1.0 - errorModel->GetChunkSuccessRate (snrDb, nbits, bandWidth, sf, codeRate);
    NS_ASSERT (perForDR >= 0.0 && perForDR <= 1.0);

    if (perForDR <= m_drCalcPerLimit) {
      calculatedDataRateIndex = it->dataRateIndex;
      //hitPerLimit = true;
      break;
    }
  }

  //Vector endDevicePos = endDeviceMobility->GetPosition ();
  //double distance = CalculateDistance (endDeviceMobility->GetPosition (), posGw);

  return calculatedDataRateIndex;
}

uint8_t LpwanTracing::CalculateRandomDataRateIndex (Ptr<Application> endDeviceApp){
  const Ptr<Node> endDeviceNode = endDeviceApp->GetNode ();

  Ptr<UniformRandomVariable> dataRateRandomVariable = CreateObject<UniformRandomVariable> ();
  uint8_t calculatedDataRateIndex = dataRateRandomVariable->GetInteger (0, LoRaWAN::m_supportedDataRates.size() - 2);
  //std::cout << "CalculateRandomDataRateIndex: node " << endDeviceNode->GetId () << "\tDRindex = " << (unsigned int)calculatedDataRateIndex
  //  << "\tSF=" << (unsigned int)LoRaWAN::m_supportedDataRates[calculatedDataRateIndex].spreadingFactor << std::endl;
  return calculatedDataRateIndex;
}

uint8_t LpwanTracing::CalculateFixedDataRateIndex (Ptr<Application> endDeviceApp) {
  const Ptr<Node> endDeviceNode = endDeviceApp->GetNode ();

  uint8_t calculatedDataRateIndex = m_fixedDataRateIndex;
  //if (m_verbose) {
  //  std::cout << "CalculateFixedDataRateIndex: node " << endDeviceNode->GetId () << "\tDRindex = " << (unsigned int)calculatedDataRateIndex
  //    << "\tSF=" << (unsigned int)LoRaWAN::m_supportedDataRates[calculatedDataRateIndex].spreadingFactor << std::endl;
  //}
  return calculatedDataRateIndex;
}


void LpwanTracing::NbInstallApplications() {


  InternetStackHelper internet;
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
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
  internet.Install (m_nbEndDeviceNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (m_nbEDDevices));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < m_nbEndDeviceNodes.GetN (); ++u)
  {
    Ptr<Node> ueNode = m_nbEndDeviceNodes.Get (u);
    // Set the default gateway for the UE
    Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
    ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
  }

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < m_nNbDevices; i++)
  {
    lteHelper->Attach (m_nbEDDevices.Get(i), m_nbGWDevices.Get(i));
    // side effect: the default EPS bearer will be activated
  }


  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1234;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < m_nbEndDeviceNodes.GetN (); ++u)
  {
    ++ulPort;
    ++otherPort;
    PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
    PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
    PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
    serverApps.Add (dlPacketSinkHelper.Install (m_nbEndDeviceNodes.Get(u)));
    serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
    serverApps.Add (packetSinkHelper.Install (m_nbEndDeviceNodes.Get(u)));

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
    clientApps.Add (ulClient.Install (m_nbEndDeviceNodes.Get(u)));
    if (u+1 < m_nbEndDeviceNodes.GetN ())
    {
      clientApps.Add (client.Install (m_nbEndDeviceNodes.Get(u+1)));
    }
    else
    {
      clientApps.Add (client.Install (m_nbEndDeviceNodes.Get(0)));
    }
  }
  serverApps.Start (Seconds (0.01));
  clientApps.Start (Seconds (0.01));
}



