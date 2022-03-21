
//  ==> COPYRIGHT (C) 2019 XSENS TECHNOLOGIES OR SUBSIDIARIES WORLDWIDE <==
//  WARNING: COPYRIGHT (C) 2019 XSENS TECHNOLOGIES OR SUBSIDIARIES WORLDWIDE. ALL RIGHTS RESERVED.
//  THIS FILE AND THE SOURCE CODE IT CONTAINS (AND/OR THE BINARY CODE FILES FOUND IN THE SAME
//  FOLDER THAT CONTAINS THIS FILE) AND ALL RELATED SOFTWARE (COLLECTIVELY, "CODE") ARE SUBJECT
//  TO AN END USER LICENSE AGREEMENT ("AGREEMENT") BETWEEN XSENS AS LICENSOR AND THE AUTHORIZED
//  LICENSEE UNDER THE AGREEMENT. THE CODE MUST BE USED SOLELY WITH XSENS PRODUCTS INCORPORATED
//  INTO LICENSEE PRODUCTS IN ACCORDANCE WITH THE AGREEMENT. ANY USE, MODIFICATION, COPYING OR
//  DISTRIBUTION OF THE CODE IS STRICTLY PROHIBITED UNLESS EXPRESSLY AUTHORIZED BY THE AGREEMENT.
//  IF YOU ARE NOT AN AUTHORIZED USER OF THE CODE IN ACCORDANCE WITH THE AGREEMENT, YOU MUST STOP
//  USING OR VIEWING THE CODE NOW, REMOVE ANY COPIES OF THE CODE FROM YOUR COMPUTER AND NOTIFY
//  XSENS IMMEDIATELY BY EMAIL TO INFO@XSENS.COM. ANY COPIES OR DERIVATIVES OF THE CODE (IN WHOLE
//  OR IN PART) IN SOURCE CODE FORM THAT ARE PERMITTED BY THE AGREEMENT MUST RETAIN THE ABOVE
//  COPYRIGHT NOTICE AND THIS PARAGRAPH IN ITS ENTIRETY, AS REQUIRED BY THE AGREEMENT.
//  
//  THIS SOFTWARE CAN CONTAIN OPEN SOURCE COMPONENTS WHICH CAN BE SUBJECT TO 
//  THE FOLLOWING GENERAL PUBLIC LICENSES:
//  ==> Qt GNU LGPL version 3: http://doc.qt.io/qt-5/lgpl.html <==
//  ==> LAPACK BSD License:  http://www.netlib.org/lapack/LICENSE.txt <==
//  ==> StackWalker 3-Clause BSD License: https://github.com/JochenKalmbach/StackWalker/blob/master/LICENSE <==
//  ==> Icon Creative Commons 3.0: https://creativecommons.org/licenses/by/3.0/legalcode <==
//  

//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 3.0.10
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------

namespace XDA {

public enum XsDataIdentifier {
  XDI_None = 0x0000,
  XDI_TypeMask = 0xFE00,
  XDI_FullTypeMask = 0xFFF0,
  XDI_FullMask = 0xFFFF,
  XDI_FormatMask = 0x01FF,
  XDI_DataFormatMask = 0x000F,
  XDI_SubFormatMask = 0x0003,
  XDI_SubFormatFloat = 0x0000,
  XDI_SubFormatFp1220 = 0x0001,
  XDI_SubFormatFp1632 = 0x0002,
  XDI_SubFormatDouble = 0x0003,
  XDI_TemperatureGroup = 0x0800,
  XDI_Temperature = 0x0810,
  XDI_TimestampGroup = 0x1000,
  XDI_UtcTime = 0x1010,
  XDI_PacketCounter = 0x1020,
  XDI_Itow = 0x1030,
  XDI_GnssAge = 0x1040,
  XDI_PressureAge = 0x1050,
  XDI_SampleTimeFine = 0x1060,
  XDI_SampleTimeCoarse = 0x1070,
  XDI_FrameRange = 0x1080,
  XDI_PacketCounter8 = 0x1090,
  XDI_SampleTime64 = 0x10A0,
  XDI_OrientationGroup = 0x2000,
  XDI_CoordSysMask = 0x000C,
  XDI_CoordSysEnu = 0x0000,
  XDI_CoordSysNed = 0x0004,
  XDI_CoordSysNwu = 0x0008,
  XDI_Quaternion = 0x2010,
  XDI_RotationMatrix = 0x2020,
  XDI_EulerAngles = 0x2030,
  XDI_PressureGroup = 0x3000,
  XDI_BaroPressure = 0x3010,
  XDI_AccelerationGroup = 0x4000,
  XDI_DeltaV = 0x4010,
  XDI_Acceleration = 0x4020,
  XDI_FreeAcceleration = 0x4030,
  XDI_AccelerationHR = 0x4040,
  XDI_PositionGroup = 0x5000,
  XDI_AltitudeMsl = 0x5010,
  XDI_AltitudeEllipsoid = 0x5020,
  XDI_PositionEcef = 0x5030,
  XDI_LatLon = 0x5040,
  XDI_SnapshotGroup = 0xC800,
  XDI_RetransmissionMask = 0x0001,
  XDI_RetransmissionFlag = 0x0001,
  XDI_AwindaSnapshot = 0xC810,
  XDI_FullSnapshot = 0xC820,
  XDI_GloveSnapshot = 0xC830,
  XDI_GloveData = 0xC840,
  XDI_GnssGroup = 0x7000,
  XDI_GnssPvtData = 0x7010,
  XDI_GnssSatInfo = 0x7020,
  XDI_AngularVelocityGroup = 0x8000,
  XDI_RateOfTurn = 0x8020,
  XDI_DeltaQ = 0x8030,
  XDI_RateOfTurnHR = 0x8040,
  XDI_RawSensorGroup = 0xA000,
  XDI_RawUnsigned = 0x0000,
  XDI_RawSigned = 0x0001,
  XDI_RawAccGyrMagTemp = 0xA010,
  XDI_RawGyroTemp = 0xA020,
  XDI_RawAcc = 0xA030,
  XDI_RawGyr = 0xA040,
  XDI_RawMag = 0xA050,
  XDI_RawDeltaQ = 0xA060,
  XDI_RawDeltaV = 0xA070,
  XDI_RawBlob = 0xA080,
  XDI_AnalogInGroup = 0xB000,
  XDI_AnalogIn1 = 0xB010,
  XDI_AnalogIn2 = 0xB020,
  XDI_MagneticGroup = 0xC000,
  XDI_MagneticField = 0xC020,
  XDI_MagneticFieldCorrected = 0xC030,
  XDI_VelocityGroup = 0xD000,
  XDI_VelocityXYZ = 0xD010,
  XDI_StatusGroup = 0xE000,
  XDI_StatusByte = 0xE010,
  XDI_StatusWord = 0xE020,
  XDI_Rssi = 0xE040,
  XDI_DeviceId = 0xE080,
  XDI_LocationId = 0xE090,
  XDI_IndicationGroup = 0x4800,
  XDI_TriggerIn1 = 0x4810,
  XDI_TriggerIn2 = 0x4820
}

}