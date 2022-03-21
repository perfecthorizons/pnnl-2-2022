
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

public class XsStringArrayImpl : global::System.IDisposable {
  private global::System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal XsStringArrayImpl(global::System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
  }

  internal static global::System.Runtime.InteropServices.HandleRef getCPtr(XsStringArrayImpl obj) {
    return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~XsStringArrayImpl() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != global::System.IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          xsensdeviceapiPINVOKE.delete_XsStringArrayImpl(swigCPtr);
        }
        swigCPtr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
      }
      global::System.GC.SuppressFinalize(this);
    }
  }

  public XsStringArrayImpl(uint count, XsString src) : this(xsensdeviceapiPINVOKE.new_XsStringArrayImpl__SWIG_0(count, XsString.getCPtr(src)), true) {
  }

  public XsStringArrayImpl(uint count) : this(xsensdeviceapiPINVOKE.new_XsStringArrayImpl__SWIG_1(count), true) {
  }

  public XsStringArrayImpl() : this(xsensdeviceapiPINVOKE.new_XsStringArrayImpl__SWIG_2(), true) {
  }

  public XsStringArrayImpl(XsStringArrayImpl other) : this(xsensdeviceapiPINVOKE.new_XsStringArrayImpl__SWIG_3(XsStringArrayImpl.getCPtr(other)), true) {
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public XsStringArrayImpl(XsString arg0, uint sz, XsDataFlags flags) : this(xsensdeviceapiPINVOKE.new_XsStringArrayImpl__SWIG_4(XsString.getCPtr(arg0), sz, (int)flags), true) {
  }

  public void clear() {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_clear(swigCPtr);
  }

  public void reserve(uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_reserve(swigCPtr, count);
  }

  public uint reserved() {
    uint ret = xsensdeviceapiPINVOKE.XsStringArrayImpl_reserved(swigCPtr);
    return ret;
  }

  public SWIGTYPE_p_XsArrayDescriptor descriptor() {
    SWIGTYPE_p_XsArrayDescriptor ret = new SWIGTYPE_p_XsArrayDescriptor(xsensdeviceapiPINVOKE.XsStringArrayImpl_descriptor(swigCPtr), false);
    return ret;
  }

  public XsString value(uint index) {
    XsString ret = new XsString(xsensdeviceapiPINVOKE.XsStringArrayImpl_value(swigCPtr, index), true);
    return ret;
  }

  public XsString first() {
    XsString ret = new XsString(xsensdeviceapiPINVOKE.XsStringArrayImpl_first(swigCPtr), true);
    return ret;
  }

  public XsString last() {
    XsString ret = new XsString(xsensdeviceapiPINVOKE.XsStringArrayImpl_last(swigCPtr), true);
    return ret;
  }

  public XsString at(uint index) {
    XsString ret = new XsString(xsensdeviceapiPINVOKE.XsStringArrayImpl_at__SWIG_0(swigCPtr, index), false);
    return ret;
  }

  public void insert(XsString item, uint index) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_insert__SWIG_0(swigCPtr, XsString.getCPtr(item), index);
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public void insert(XsString items, uint index, uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_insert__SWIG_1(swigCPtr, XsString.getCPtr(items), index, count);
  }

  public void push_back(XsString item) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_push_back(swigCPtr, XsString.getCPtr(item));
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public void pop_back(uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_pop_back__SWIG_0(swigCPtr, count);
  }

  public void pop_back() {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_pop_back__SWIG_1(swigCPtr);
  }

  public void push_front(XsString item) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_push_front(swigCPtr, XsString.getCPtr(item));
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public void pop_front(uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_pop_front__SWIG_0(swigCPtr, count);
  }

  public void pop_front() {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_pop_front__SWIG_1(swigCPtr);
  }

  public uint size() {
    uint ret = xsensdeviceapiPINVOKE.XsStringArrayImpl_size(swigCPtr);
    return ret;
  }

  public void erase(uint index, uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_erase__SWIG_0(swigCPtr, index, count);
  }

  public void erase(uint index) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_erase__SWIG_1(swigCPtr, index);
  }

  public void assign(uint count, XsString src) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_assign(swigCPtr, count, XsString.getCPtr(src));
  }

  public void resize(uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_resize(swigCPtr, count);
  }

  public void setSize(uint count) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_setSize(swigCPtr, count);
  }

  public void append(XsStringArrayImpl other) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_append(swigCPtr, XsStringArrayImpl.getCPtr(other));
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool empty() {
    bool ret = xsensdeviceapiPINVOKE.XsStringArrayImpl_empty(swigCPtr);
    return ret;
  }

  public void swap(XsStringArrayImpl other) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_swap(swigCPtr, XsStringArrayImpl.getCPtr(other));
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
  }

  public int find(XsString needle) {
    int ret = xsensdeviceapiPINVOKE.XsStringArrayImpl_find(swigCPtr, XsString.getCPtr(needle));
    if (xsensdeviceapiPINVOKE.SWIGPendingException.Pending) throw xsensdeviceapiPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void removeDuplicates() {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_removeDuplicates(swigCPtr);
  }

  public void removeDuplicatesPredicate(SWIGTYPE_p_f_p_q_const__void_p_q_const__void__int predicate) {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_removeDuplicatesPredicate(swigCPtr, SWIGTYPE_p_f_p_q_const__void_p_q_const__void__int.getCPtr(predicate));
  }

  public void sort() {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_sort(swigCPtr);
  }

  public void reverse() {
    xsensdeviceapiPINVOKE.XsStringArrayImpl_reverse(swigCPtr);
  }

}

}
