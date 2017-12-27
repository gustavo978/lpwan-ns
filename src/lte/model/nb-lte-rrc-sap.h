/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Universita' degli Studi di Firenze (UNIFI)
 *
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
 *
 * Author: Samuele Foni <samuele.foni@stud.unifi.it> (NB-IOT)
 */

#ifndef SRC_LTE_MODEL_NB_LTE_RRC_SAP_H_
#define SRC_LTE_MODEL_NB_LTE_RRC_SAP_H_

#include <stdint.h>
#include <string>

namespace ns3 {

/*
 * This class goal is to define the structs needed to the NB-IOT system.
 *
 * The NB-LTE fork is the first name of the NB-IOT standard, as specified
 * by the 3GPP release 12.
 */

class NbLteRrcSap {
public:
  NbLteRrcSap();
  virtual ~NbLteRrcSap();

  // BEGIN structs for MIB-NB

  struct ChannelRasterOffsetNb{
    enum
    {
      khz_7dot5,
      khz_2dot5,
      khz2dot5,
      khz7dot5

    } cr;
  };

  static double ConvertChannelRasterOffsetNb2Double (ChannelRasterOffsetNb channelRasterOffset)
  {
    double res = 0;
    switch (channelRasterOffset.cr)
    {
      case ChannelRasterOffsetNb::khz_7dot5:
        res = -7.5;
        break;

      case ChannelRasterOffsetNb::khz_2dot5:
        res = -2.5;
        break;

      case ChannelRasterOffsetNb::khz2dot5:
        res = 2.5;
        break;

      case ChannelRasterOffsetNb::khz7dot5:
        res = 7.5;
        break;
      default:
        break;
    }
    return res;
  }

  struct InbandSamePciNb
  {
    uint8_t eutraCrsSequenceInfo;
  };

  struct InbandDifferentPciNb
  {
    uint8_t eutraNumberCrsPorts;
    ChannelRasterOffsetNb rasterOffset;
    //spare bit string size 2;
  };

  struct GuardbandNb
  {
    ChannelRasterOffsetNb rasterOffset;
    //spare bit string size 3;
  };

  struct StandaloneNb
  {
    //spare bit string size 5;
  };

  struct OperationModeInfo
  {
    InbandSamePciNb inbandSamePci;
    InbandDifferentPciNb inbandDifferentPci;
    GuardbandNb guardband;
    StandaloneNb standalone;
  };

  // END structs for MIB-NB

  // BEGIN structs for SIB1

  struct CellAccessRelatedInfoR13 {
    uint32_t cellIdentity;
  };

  struct CellSelectionInfoR13
  {
    int8_t qRxLevMin; ///< INTEGER (-70..-22), actual value = IE value * 2 [dBm].
    int8_t qQualMin; ///< INTEGER (-34..-3), actual value = IE value [dB].

  };

  struct PMax {};
  struct NsPmaxListNb {};
  struct MultiBandInfoListNb {};
  struct DlBitmapNb {};

  struct EutraControlRegionSize
  {
    enum {
      N1,
      N2,
      N3
    }ecrs; // Number of OFDM signals
  };

  struct NrsCrsPowerOffset
  {
    enum {
      dB_6,
      dB_4dot77,
      dB_3,
      dB_1dot77,
      dB0,
      dB1,
      dB1dot23,
      dB2,
      dB3,
      dB4,
      dB4dot23,
      dB5,
      dB6,
      dB7,
      dB8,
      dB9
    }crs;
  };

  static double ConvertNrsCrsPowerOffset2Double (NrsCrsPowerOffset nrsCrsPowerOffset)
  {
    double res = 0;
    switch (nrsCrsPowerOffset.crs)
    {
      case NrsCrsPowerOffset::dB_6:
        res = -6.0;
        break;

      case NrsCrsPowerOffset::dB_4dot77:
        res = -4.77;
        break;

      case NrsCrsPowerOffset::dB_3:
        res = -3.0;
        break;

      case NrsCrsPowerOffset::dB_1dot77:
        res = -1.77;
        break;

      case NrsCrsPowerOffset::dB0:
        res = 0.0;
        break;

      case NrsCrsPowerOffset::dB1:
        res = 1.0;
        break;

      case NrsCrsPowerOffset::dB1dot23:
        res = 1.23;
        break;

      case NrsCrsPowerOffset::dB2:
        res = 2.0;
        break;

      case NrsCrsPowerOffset::dB3:
        res = 3.0;
        break;

      case NrsCrsPowerOffset::dB4:
        res = 4.0;
        break;

      case NrsCrsPowerOffset::dB4dot23:
        res = 4.23;
        break;

      case NrsCrsPowerOffset::dB5:
        res = 5.0;
        break;

      case NrsCrsPowerOffset::dB6:
        res = 6.0;
        break;

      case NrsCrsPowerOffset::dB7:
        res = 7.0;
        break;

      case NrsCrsPowerOffset::dB8:
        res = 8.0;
        break;

      case NrsCrsPowerOffset::dB9:
        res = 9.0;
        break;

      default:
        break;
    }
    return res;
  }

  struct SchedulingInfoList {
    uint32_t siTb;
  };

  struct SiWindowLength
  {
    enum
    {
      ms160,
      ms320,
      ms480,
      ms640,
      ms960,
      ms1280,
      ms1600,
      spare1
    } swl;
  };

  static double ConvertSiWindowLength2Double (SiWindowLength siWindowLength)
  {
    double res = 0;
    switch (siWindowLength.swl)
    {
      case SiWindowLength::ms160:
        res = 160.0;
        break;

      case SiWindowLength::ms320:
        res = 320.0;
        break;

      case SiWindowLength::ms480:
        res = 480.0;
        break;

      case SiWindowLength::ms640:
        res = 640.0;
        break;

      case SiWindowLength::ms960:
        res = 960.0;
        break;

      case SiWindowLength::ms1280:
        res = 1280.0;
        break;

      case SiWindowLength::ms1600:
        res = 1600.0;
        break;

      default:
        break;
    }
    return res;
  }

  struct SiRadioFrameOffset {};
  struct SystemInfoValueTagList {};
  struct LateNonCriticalExtension {};
  struct NonCriticalExtension {};

  // END structs for SIB1

  // BEGIN structs for SIB2

  enum PdcchPeriods // Number of PDCCH periods
  {
    pp1,
    pp2,
    pp3,
    pp4,
    pp5,
    pp6,
    pp7,
    pp8,
    pp10,
    pp16,
    pp32,
    pp64
  };

  struct RachInfoNb
  {
    PdcchPeriods raResponseWindowSize; // List of possibles enable values: pp2, pp3, pp4, pp5, pp6, pp7, pp8, pp10

    PdcchPeriods macContentionResolutionTimer; // List of possibles enable values: pp1, pp2, pp3, pp4, pp8, pp16, pp32, pp64
  };

  struct NprachParametersNb
  {
    enum
    {
      MS40,
      MS80,
      MS160,
      MS240,
      MS320,
      MS640,
      MS1280,
      MS2560
    } nprachPeriodicity;

    enum
    {
      ms8,
      ms16,
      ms32,
      ms64,
      ms128,
      ms256,
      ms512,
      ms1024
    } nprachStartTime;


    enum NumberOfSubcarriers
    {
      n0,
      n2,
      n12,
      n18,
      n24,
      n34,
      n36,
      n48,
      spare1
    };

    NumberOfSubcarriers nprachSubcarrierOffset; // List of possibles enable values: n0, n12, n24, n36, n2, n18, n34, spare1

    NumberOfSubcarriers nprachNumSubcarriers; // List of possibles enable values: n12, n24, n36, n48


    enum
    {
      zero,
      oneThird,
      twoThird,
      one
    } nprachSubcarrierMsg3RangeStart;


    enum NumberOfPreambles
    {
      N1,
      N2,
      N3,
      N4,
      N5,
      N6,
      N7,
      N8,
      N10,
      N16,
      N32,
      N64,
      N128,
      SPARE1
    };


    NumberOfPreambles maxNumPreambleAttemptCe;  // Maximum number of preamble transmission attempts per NPRACH resource. See TS 36.321 [6].
                                                // List of possibles enable values: n3, n4, n5, n6, n7, n8, n10, spare1


    NumberOfPreambles numRepetitionsPerPreambleAttempt; // Number of NPRACH repetitions per attempt for each NPRACH resource, See TS 36.211 [21, 10.1.6].
                                                        // List of possibles enable values: n1, n2, n4, n8, n16, n32, n64, n128


    enum
    {
      r1,
      r2,
      r4,
      r8,
      r16,
      r32,
      r64,
      r128,
      r256,
      r512,
      r1024,
      r2048,
      spare_4,
      spare_3,
      spare_2,
      spare_1
    } npdcchNumRepetitionsRa;

    enum
    {
      v1dot5,
      v2,
      v4,
      v8,
      v16,
      v32,
      v48,
      v64
    } npdcchStartSfCssRa;

    enum
    {
      ZERO,
      ONE_EIGHTH,
      ONE_FOURTH,
      THREE_EIGHTH
    } npdcchOffsetRa;
  }; // End of struct NprachParametersNb

  struct RsrpThresholdsNprachInfoListNb
  {
    // SEQUENCE (SIZE[1..2]) OF RsrpRange
  };

  struct NprachParametersListNb
  {
    // SEQUENCE (SIZE [1.. max NprachResourcesNb])OF NprachParametersNb
  };

  struct NprachConfigSibNb
  {
    enum
    {
      us66dot7,
      us266dot7
    } nprachCpLength;
    RsrpThresholdsNprachInfoListNb rsrpThresholdsPrachInfoList;
    NprachParametersListNb nprachParametersList;
  };

  struct PowerRampingParameters {
    enum
    {
      dB0,
      dB2,
      dB4,
      dB6
    } powerRampingStep;

    enum
    {
      dBm_120,
      dBm_118,
      dBm_116,
      dBm_114,
      dBm_112,
      dBm_110,
      dBm_108,
      dBm_106,
      dBm_104,
      dBm_102,
      dBm_100,
      dBm_98,
      dBm_96,
      dBm_94,
      dBm_92,
      dBm_90
    } preambleInitialReceivedTargetPower;
  };

  struct RachConfigCommonNb
  {
    //PreambleInfo preambleInfo;
    //RaSupervisionInfo raSupervisionInfo;
    PowerRampingParameters powerRampingParameters;
    uint8_t rachInfoListNb; // SEQUENCE [SIZE (1 .. max NprachResourcesNb)] OF RachInfoNb
    uint8_t connEstFailOffset; // --Need OP.

  };

  struct PdschConfigCommon
  {
    int8_t referenceSignalPower;  // INTEGER (-60..50),
    int8_t pb;                    // INTEGER (0..3),
  };

  struct RadioResourceConfigCommonSibNb
  {
    RachConfigCommonNb rachConfigCommonNb;

    //bcch-Config-r13                         BCCH-Config-NB-r13,
    //pcch-Config-r13                         PCCH-Config-NB-r13,

    NprachConfigSibNb nprachConfig;

    // npdsch-ConfigCommon-r13                 NPDSCH-ConfigCommon-NB-r13,
    //npusch-ConfigCommon-r13                 NPUSCH-ConfigCommon-NB-r13,
    //dl-Gap-r13                              DL-GapConfig-NB-r13
    //uplinkPowerControlCommon-r13            UplinkPowerControlCommon-NB-r13,
  };

  struct UeTimersAndConstantsNb {};

  struct FreqInfo
  {
    uint16_t ulCarrierFreqNb;
    uint8_t ulBandwidth; // Not needed in this context.
    uint8_t additionalSpectrumEmission; // May be unused...
  };

  struct TimeAlignmentTimer {};
  struct MultiBandInfoList {};

  // END structs for SIB2

  /*
   * MasterInformationBlock-NB also known as MIB-NB carries relatively large set of information on an LTE-NB environment,
   * while in LTE its structure is very simple. This feature represent one of the best differences between LTE-NB and
   * legacy LTE.
   */


  struct MasterInformationBlockNb
  {
    uint8_t systemFrameNumberMsb;
    uint8_t hyperSfnLsb;
    uint8_t schedulingInfoSib1;
    bool abEnabled;
    OperationModeInfo operationMode;
    //spare bit string size 11;
  };

  struct SystemInformationBlockType1Nb
  {
    uint8_t hyperSfnMsbR13;
    CellAccessRelatedInfoR13 cellAccessRelatedInfo;
    CellSelectionInfoR13 cellSelectionInfo;
    PMax pMaxR13; // Need OP  -- not used in this context
    int8_t freqbandIndicator; // FBI = [1...64] -- not used in this context
    NsPmaxListNb freqBandInfo;  // -- Need OR
    MultiBandInfoListNb multiBandInfoList;  // -- Need OR
    DlBitmapNb downlinkBitmap;  //  -- Need OP
    EutraControlRegionSize eutraControlRegionSize;  // Optional. This value is applied only to in-band Operation mode.
    //It indicates how many OFDM symbols are used for control region.
    NrsCrsPowerOffset nrsCrsPowerOffset;  // Optional. This value is applied only to in-band Operation mode (Same PCI).
    SchedulingInfoList schedulingInfoListNb;
    SiWindowLength siWindowLength;
    SiRadioFrameOffset siRadioFrameOffset;  // -- Need OP
    SystemInfoValueTagList systemInfoValueTagList;  // -- Need OR
    LateNonCriticalExtension lateNonCriticalExtension;  // Optional. Not needed.
    NonCriticalExtension nonCriticalExtension;  // Optional. Not needed.
  };

  struct SystemInformationBlockType2Nb
  {
    RadioResourceConfigCommonSibNb radioResourceConfigCommonSibNb;
    UeTimersAndConstantsNb ueTimersAndConstantsNb;
    FreqInfo freqInfo;
    TimeAlignmentTimer timeAlignmentTimer;
    MultiBandInfoList multiBandInfoList;
    LateNonCriticalExtension lateNonCriticalExtension;
  };

};

} /* namespace ns3 */

#endif /* SRC_LTE_MODEL_NB_LTE_RRC_SAP_H_ */
