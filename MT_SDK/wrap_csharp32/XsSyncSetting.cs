
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

public class XsSyncSetting : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal XsSyncSetting(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(XsSyncSetting obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~XsSyncSetting() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          xsensdeviceapiPINVOKE.delete_XsSyncSetting(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public XsSyncLine m_line {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_line_set(swigCPtr, (int)value);
    } 
    get {
      XsSyncLine ret = (XsSyncLine)xsensdeviceapiPINVOKE.XsSyncSetting_m_line_get(swigCPtr);
      return ret;
    } 
  }

  public XsSyncFunction m_function {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_function_set(swigCPtr, (int)value);
    } 
    get {
      XsSyncFunction ret = (XsSyncFunction)xsensdeviceapiPINVOKE.XsSyncSetting_m_function_get(swigCPtr);
      return ret;
    } 
  }

  public XsSyncPolarity m_polarity {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_polarity_set(swigCPtr, (int)value);
    } 
    get {
      XsSyncPolarity ret = (XsSyncPolarity)xsensdeviceapiPINVOKE.XsSyncSetting_m_polarity_get(swigCPtr);
      return ret;
    } 
  }

  public uint m_pulseWidth {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_pulseWidth_set(swigCPtr, value);
    } 
    get {
      uint ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_pulseWidth_get(swigCPtr);
      return ret;
    } 
  }

  public int m_offset {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_offset_set(swigCPtr, value);
    } 
    get {
      int ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_offset_get(swigCPtr);
      return ret;
    } 
  }

  public ushort m_skipFirst {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_skipFirst_set(swigCPtr, value);
    } 
    get {
      ushort ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_skipFirst_get(swigCPtr);
      return ret;
    } 
  }

  public ushort m_skipFactor {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_skipFactor_set(swigCPtr, value);
    } 
    get {
      ushort ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_skipFactor_get(swigCPtr);
      return ret;
    } 
  }

  public ushort m_clockPeriod {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_clockPeriod_set(swigCPtr, value);
    } 
    get {
      ushort ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_clockPeriod_get(swigCPtr);
      return ret;
    } 
  }

  public byte m_triggerOnce {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_triggerOnce_set(swigCPtr, value);
    } 
    get {
      byte ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_triggerOnce_get(swigCPtr);
      return ret;
    } 
  }

  public byte m_padding {
    set {
      xsensdeviceapiPINVOKE.XsSyncSetting_m_padding_set(swigCPtr, value);
    } 
    get {
      byte ret = xsensdeviceapiPINVOKE.XsSyncSetting_m_padding_get(swigCPtr);
      return ret;
    } 
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity, uint pulseWidth, int offset, ushort skipFirst, ushort skipFactor, ushort clockPeriod, byte triggerOnce) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_0((int)line, (int)function, (int)polarity, pulseWidth, offset, skipFirst, skipFactor, clockPeriod, triggerOnce), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity, uint pulseWidth, int offset, ushort skipFirst, ushort skipFactor, ushort clockPeriod) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_1((int)line, (int)function, (int)polarity, pulseWidth, offset, skipFirst, skipFactor, clockPeriod), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity, uint pulseWidth, int offset, ushort skipFirst, ushort skipFactor) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_2((int)line, (int)function, (int)polarity, pulseWidth, offset, skipFirst, skipFactor), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity, uint pulseWidth, int offset, ushort skipFirst) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_3((int)line, (int)function, (int)polarity, pulseWidth, offset, skipFirst), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity, uint pulseWidth, int offset) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_4((int)line, (int)function, (int)polarity, pulseWidth, offset), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity, uint pulseWidth) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_5((int)line, (int)function, (int)polarity, pulseWidth), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function, XsSyncPolarity polarity) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_6((int)line, (int)function, (int)polarity), true) {
  }

  public XsSyncSetting(XsSyncLine line, XsSyncFunction function) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_7((int)line, (int)function), true) {
  }

  public XsSyncSetting(XsSyncLine line) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_8((int)line), true) {
  }

  public XsSyncSetting() : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_9(), true) {
  }

  public XsSyncSetting(XsSyncSetting other) : this(xsensdeviceapiPINVOKE.new_XsSyncSetting__SWIG_10(XsSyncSetting.getCPtr(other)), true) {
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool isInput() {
    bool ret = xsensdeviceapiPINVOKE.XsSyncSetting_isInput(swigCPtr);
    return ret;
  }

  public bool isOutput() {
    bool ret = xsensdeviceapiPINVOKE.XsSyncSetting_isOutput(swigCPtr);
    return ret;
  }

  public void swap(XsSyncSetting other) {
    xsensdeviceapiPINVOKE.XsSyncSetting_swap(swigCPtr, XsSyncSetting.getCPtr(other));
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

}

}
