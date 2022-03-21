
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

public class XsCallbackPlainC : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal XsCallbackPlainC(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(XsCallbackPlainC obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          throw new global::System.MethodAccessException("C++ destructor does not have public access");
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsDeviceState_enum_XsDeviceState__void m_onDeviceStateChanged {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onDeviceStateChanged_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsDeviceState_enum_XsDeviceState__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onDeviceStateChanged_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsDeviceState_enum_XsDeviceState__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsDeviceState_enum_XsDeviceState__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void m_onLiveDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onLiveDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onLiveDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_int__void m_onMissedPackets {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMissedPackets_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_int__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMissedPackets_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_int__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_int__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice__void m_onWakeupReceived {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onWakeupReceived_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onWakeupReceived_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_p_q_const__XsString__void m_onProgressUpdated {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onProgressUpdated_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_p_q_const__XsString__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onProgressUpdated_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_p_q_const__XsString__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_int_int_p_q_const__XsString__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__int m_onWriteMessageToLogFile {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onWriteMessageToLogFile_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__int.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onWriteMessageToLogFile_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__int ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__int(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void m_onBufferedDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onBufferedDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onBufferedDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsConnectivityState__void m_onConnectivityChanged {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onConnectivityChanged_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsConnectivityState__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onConnectivityChanged_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsConnectivityState__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsConnectivityState__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsInfoRequest__void m_onInfoResponse {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onInfoResponse_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsInfoRequest__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onInfoResponse_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsInfoRequest__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsInfoRequest__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsResultValue__void m_onError {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onError_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsResultValue__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onError_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsResultValue__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsResultValue__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void m_onNonDataMessage {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onNonDataMessage_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onNonDataMessage_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsProtocolType_p_q_const__XsByteArray__void m_onMessageDetected {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMessageDetected_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsProtocolType_p_q_const__XsByteArray__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMessageDetected_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsProtocolType_p_q_const__XsByteArray__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_enum_XsProtocolType_p_q_const__XsByteArray__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void m_onMessageReceivedFromDevice {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMessageReceivedFromDevice_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMessageReceivedFromDevice_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void m_onMessageSentToDevice {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMessageSentToDevice_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onMessageSentToDevice_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsMessage__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void m_onAllLiveDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllLiveDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllLiveDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void m_onAllBufferedDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllBufferedDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllBufferedDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_long_long__void m_onDataUnavailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onDataUnavailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_long_long__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onDataUnavailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_long_long__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_long_long__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void m_onDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void m_onAllDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void m_onRecordedDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onRecordedDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onRecordedDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevice_p_q_const__XsDataPacket__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void m_onAllRecordedDataAvailable {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllRecordedDataAvailable_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onAllRecordedDataAvailable_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_XsDevicePtrArray_p_q_const__XsDataPacketPtrArray__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_int_p_q_const__XsByteArray__void m_onTransmissionRequest {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onTransmissionRequest_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_int_p_q_const__XsByteArray__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onTransmissionRequest_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_int_p_q_const__XsByteArray__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_int_p_q_const__XsByteArray__void(cPtr, false);
      return ret;
    } 
  }

  public SWIGTYPE_p_f_p_XsCallbackPlainC_p_q_const__XsString_enum_XsResultValue__void m_onRestoreCommunication {
    set {
      xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onRestoreCommunication_set(swigCPtr, SWIGTYPE_p_f_p_XsCallbackPlainC_p_q_const__XsString_enum_XsResultValue__void.getCPtr(value));
    } 
    get {
      global::System.IntPtr cPtr = xsensdeviceapiPINVOKE.XsCallbackPlainC_m_onRestoreCommunication_get(swigCPtr);
      SWIGTYPE_p_f_p_XsCallbackPlainC_p_q_const__XsString_enum_XsResultValue__void ret = (cPtr == global::System.IntPtr.Zero) ? null : new SWIGTYPE_p_f_p_XsCallbackPlainC_p_q_const__XsString_enum_XsResultValue__void(cPtr, false);
      return ret;
    } 
  }

}

}
