/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.eclipse.sumo.libsumo;

public class Edge {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected Edge(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(Edge obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libsumoJNI.delete_Edge(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public static StringVector getIDList() {
    return new StringVector(libsumoJNI.Edge_getIDList(), true);
  }

  public static int getIDCount() {
    return libsumoJNI.Edge_getIDCount();
  }

  public static double getAdaptedTraveltime(String id, double time) {
    return libsumoJNI.Edge_getAdaptedTraveltime(id, time);
  }

  public static double getEffort(String id, double time) {
    return libsumoJNI.Edge_getEffort(id, time);
  }

  public static double getTraveltime(String id) {
    return libsumoJNI.Edge_getTraveltime(id);
  }

  public static double getWaitingTime(String id) {
    return libsumoJNI.Edge_getWaitingTime(id);
  }

  public static StringVector getLastStepPersonIDs(String id) {
    return new StringVector(libsumoJNI.Edge_getLastStepPersonIDs(id), true);
  }

  public static StringVector getLastStepVehicleIDs(String id) {
    return new StringVector(libsumoJNI.Edge_getLastStepVehicleIDs(id), true);
  }

  public static double getCO2Emission(String id) {
    return libsumoJNI.Edge_getCO2Emission(id);
  }

  public static double getCOEmission(String id) {
    return libsumoJNI.Edge_getCOEmission(id);
  }

  public static double getHCEmission(String id) {
    return libsumoJNI.Edge_getHCEmission(id);
  }

  public static double getPMxEmission(String id) {
    return libsumoJNI.Edge_getPMxEmission(id);
  }

  public static double getNOxEmission(String id) {
    return libsumoJNI.Edge_getNOxEmission(id);
  }

  public static double getFuelConsumption(String id) {
    return libsumoJNI.Edge_getFuelConsumption(id);
  }

  public static double getNoiseEmission(String id) {
    return libsumoJNI.Edge_getNoiseEmission(id);
  }

  public static double getElectricityConsumption(String id) {
    return libsumoJNI.Edge_getElectricityConsumption(id);
  }

  public static int getLastStepVehicleNumber(String id) {
    return libsumoJNI.Edge_getLastStepVehicleNumber(id);
  }

  public static double getLastStepMeanSpeed(String id) {
    return libsumoJNI.Edge_getLastStepMeanSpeed(id);
  }

  public static double getLastStepOccupancy(String id) {
    return libsumoJNI.Edge_getLastStepOccupancy(id);
  }

  public static int getLastStepHaltingNumber(String id) {
    return libsumoJNI.Edge_getLastStepHaltingNumber(id);
  }

  public static double getLastStepLength(String id) {
    return libsumoJNI.Edge_getLastStepLength(id);
  }

  public static int getLaneNumber(String id) {
    return libsumoJNI.Edge_getLaneNumber(id);
  }

  public static String getStreetName(String id) {
    return libsumoJNI.Edge_getStreetName(id);
  }

  public static String getParameter(String id, String paramName) {
    return libsumoJNI.Edge_getParameter(id, paramName);
  }

  public static void setAllowedVehicleClasses(String id, StringVector vector) {
    libsumoJNI.Edge_setAllowedVehicleClasses(id, StringVector.getCPtr(vector), vector);
  }

  public static void setDisallowedVehicleClasses(String id, StringVector classes) {
    libsumoJNI.Edge_setDisallowedVehicleClasses(id, StringVector.getCPtr(classes), classes);
  }

  public static void setAllowedSVCPermissions(String id, int permissions) {
    libsumoJNI.Edge_setAllowedSVCPermissions(id, permissions);
  }

  public static void adaptTraveltime(String id, double value, double begTime, double endTime) {
    libsumoJNI.Edge_adaptTraveltime__SWIG_0(id, value, begTime, endTime);
  }

  public static void adaptTraveltime(String id, double value, double begTime) {
    libsumoJNI.Edge_adaptTraveltime__SWIG_1(id, value, begTime);
  }

  public static void adaptTraveltime(String id, double value) {
    libsumoJNI.Edge_adaptTraveltime__SWIG_2(id, value);
  }

  public static void setEffort(String id, double value, double begTime, double endTime) {
    libsumoJNI.Edge_setEffort__SWIG_0(id, value, begTime, endTime);
  }

  public static void setEffort(String id, double value, double begTime) {
    libsumoJNI.Edge_setEffort__SWIG_1(id, value, begTime);
  }

  public static void setEffort(String id, double value) {
    libsumoJNI.Edge_setEffort__SWIG_2(id, value);
  }

  public static void setMaxSpeed(String id, double value) {
    libsumoJNI.Edge_setMaxSpeed(id, value);
  }

  public static void setParameter(String id, String name, String value) {
    libsumoJNI.Edge_setParameter(id, name, value);
  }

  public static void subscribe(String objID, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime, double endTime) {
    libsumoJNI.Edge_subscribe__SWIG_0(objID, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime, endTime);
  }

  public static void subscribe(String objID, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime) {
    libsumoJNI.Edge_subscribe__SWIG_1(objID, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime);
  }

  public static void subscribe(String objID, SWIGTYPE_p_std__vectorT_int_t vars) {
    libsumoJNI.Edge_subscribe__SWIG_2(objID, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars));
  }

  public static void subscribe(String objID) {
    libsumoJNI.Edge_subscribe__SWIG_3(objID);
  }

  public static void subscribeContext(String objID, int domain, double range, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime, double endTime) {
    libsumoJNI.Edge_subscribeContext__SWIG_0(objID, domain, range, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime, endTime);
  }

  public static void subscribeContext(String objID, int domain, double range, SWIGTYPE_p_std__vectorT_int_t vars, double beginTime) {
    libsumoJNI.Edge_subscribeContext__SWIG_1(objID, domain, range, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars), beginTime);
  }

  public static void subscribeContext(String objID, int domain, double range, SWIGTYPE_p_std__vectorT_int_t vars) {
    libsumoJNI.Edge_subscribeContext__SWIG_2(objID, domain, range, SWIGTYPE_p_std__vectorT_int_t.getCPtr(vars));
  }

  public static void subscribeContext(String objID, int domain, double range) {
    libsumoJNI.Edge_subscribeContext__SWIG_3(objID, domain, range);
  }

  public static SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t getAllSubscriptionResults() {
    return new SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t(libsumoJNI.Edge_getAllSubscriptionResults(), true);
  }

  public static SWIGTYPE_p_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t getSubscriptionResults(String objID) {
    return new SWIGTYPE_p_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t(libsumoJNI.Edge_getSubscriptionResults(objID), true);
  }

  public static SWIGTYPE_p_std__mapT_std__string_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t_t getAllContextSubscriptionResults() {
    return new SWIGTYPE_p_std__mapT_std__string_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t_t(libsumoJNI.Edge_getAllContextSubscriptionResults(), true);
  }

  public static SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t getContextSubscriptionResults(String objID) {
    return new SWIGTYPE_p_std__mapT_std__string_std__mapT_int_std__shared_ptrT_libsumo__TraCIResult_t_t_t(libsumoJNI.Edge_getContextSubscriptionResults(objID), true);
  }

  public static void storeShape(String id, SWIGTYPE_p_PositionVector shape) {
    libsumoJNI.Edge_storeShape(id, SWIGTYPE_p_PositionVector.getCPtr(shape));
  }

  public static SWIGTYPE_p_std__shared_ptrT_libsumo__VariableWrapper_t makeWrapper() {
    return new SWIGTYPE_p_std__shared_ptrT_libsumo__VariableWrapper_t(libsumoJNI.Edge_makeWrapper(), true);
  }

  public static boolean handleVariable(String objID, int variable, SWIGTYPE_p_libsumo__VariableWrapper wrapper) {
    return libsumoJNI.Edge_handleVariable(objID, variable, SWIGTYPE_p_libsumo__VariableWrapper.getCPtr(wrapper));
  }

}
