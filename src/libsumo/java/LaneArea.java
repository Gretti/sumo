/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.eclipse.sumo.libsumo;

public class LaneArea {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected LaneArea(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(LaneArea obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libsumoJNI.delete_LaneArea(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public static StringVector getIDList() {
    return new StringVector(libsumoJNI.LaneArea_getIDList(), true);
  }

  public static int getIDCount() {
    return libsumoJNI.LaneArea_getIDCount();
  }

  public static int getJamLengthVehicle(String detID) {
    return libsumoJNI.LaneArea_getJamLengthVehicle(detID);
  }

  public static double getJamLengthMeters(String detID) {
    return libsumoJNI.LaneArea_getJamLengthMeters(detID);
  }

  public static double getLastStepMeanSpeed(String detID) {
    return libsumoJNI.LaneArea_getLastStepMeanSpeed(detID);
  }

  public static StringVector getLastStepVehicleIDs(String detID) {
    return new StringVector(libsumoJNI.LaneArea_getLastStepVehicleIDs(detID), true);
  }

  public static double getLastStepOccupancy(String detID) {
    return libsumoJNI.LaneArea_getLastStepOccupancy(detID);
  }

  public static double getPosition(String detID) {
    return libsumoJNI.LaneArea_getPosition(detID);
  }

  public static String getLaneID(String detID) {
    return libsumoJNI.LaneArea_getLaneID(detID);
  }

  public static double getLength(String detID) {
    return libsumoJNI.LaneArea_getLength(detID);
  }

  public static int getLastStepVehicleNumber(String detID) {
    return libsumoJNI.LaneArea_getLastStepVehicleNumber(detID);
  }

  public static int getLastStepHaltingNumber(String detID) {
    return libsumoJNI.LaneArea_getLastStepHaltingNumber(detID);
  }

  public static void subscribe(String objID, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime, double endTime) {
    libsumoJNI.LaneArea_subscribe__SWIG_0(objID, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime, endTime);
  }

  public static void subscribe(String objID, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime) {
    libsumoJNI.LaneArea_subscribe__SWIG_1(objID, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime);
  }

  public static void subscribe(String objID, SWIGTYPE_p_std__vectorT_int_t vars) {
    libsumoJNI.LaneArea_subscribe__SWIG_2(objID, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars));
  }

  public static void subscribe(String objID) {
    libsumoJNI.LaneArea_subscribe__SWIG_3(objID);
  }

  public static void subscribeContext(String objID, int domain, double range, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime, double endTime) {
    libsumoJNI.LaneArea_subscribeContext__SWIG_0(objID, domain, range, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime, endTime);
  }

  public static void subscribeContext(String objID, int domain, double range, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime) {
    libsumoJNI.LaneArea_subscribeContext__SWIG_1(objID, domain, range, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime);
  }

  public static void subscribeContext(String objID, int domain, double range, SWIGTYPE_p_std__vectorT_int_t vars) {
    libsumoJNI.LaneArea_subscribeContext__SWIG_2(objID, domain, range, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars));
  }

  public static void subscribeContext(String objID, int domain, double range) {
    libsumoJNI.LaneArea_subscribeContext__SWIG_3(objID, domain, range);
  }

  public static SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t getAllSubscriptionResults() {
    return new SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t(libsumoJNI.LaneArea_getAllSubscriptionResults(), true);
  }

  public static SWIGTYPE_p_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t getSubscriptionResults(String objID) {
    return new SWIGTYPE_p_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t(libsumoJNI.LaneArea_getSubscriptionResults(objID), true);
  }

  public static SWIGTYPE_p_std__mapT_std__string_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t_t getAllContextSubscriptionResults() {
    return new SWIGTYPE_p_std__mapT_std__string_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t_t(libsumoJNI.LaneArea_getAllContextSubscriptionResults(), true);
  }

  public static SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t getContextSubscriptionResults(String objID) {
    return new SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t(libsumoJNI.LaneArea_getContextSubscriptionResults(objID), true);
  }

  public static SWIGTYPE_p_std__shared_ptrT_libsumo__VariableWrapper_t makeWrapper() {
    return new SWIGTYPE_p_std__shared_ptrT_libsumo__VariableWrapper_t(libsumoJNI.LaneArea_makeWrapper(), true);
  }

  public static boolean handleVariable(String objID, int variable, SWIGTYPE_p_libsumo__VariableWrapper wrapper) {
    return libsumoJNI.LaneArea_handleVariable(objID, variable, SWIGTYPE_p_libsumo__VariableWrapper.getCPtr(wrapper));
  }

}