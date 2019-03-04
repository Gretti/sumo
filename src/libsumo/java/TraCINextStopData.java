/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.eclipse.sumo.libsumo;

public class TraCINextStopData {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected TraCINextStopData(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(TraCINextStopData obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libsumoJNI.delete_TraCINextStopData(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setLane(String value) {
    libsumoJNI.TraCINextStopData_lane_set(swigCPtr, this, value);
  }

  public String getLane() {
    return libsumoJNI.TraCINextStopData_lane_get(swigCPtr, this);
  }

  public void setEndPos(double value) {
    libsumoJNI.TraCINextStopData_endPos_set(swigCPtr, this, value);
  }

  public double getEndPos() {
    return libsumoJNI.TraCINextStopData_endPos_get(swigCPtr, this);
  }

  public void setStoppingPlaceID(String value) {
    libsumoJNI.TraCINextStopData_stoppingPlaceID_set(swigCPtr, this, value);
  }

  public String getStoppingPlaceID() {
    return libsumoJNI.TraCINextStopData_stoppingPlaceID_get(swigCPtr, this);
  }

  public void setStopFlags(int value) {
    libsumoJNI.TraCINextStopData_stopFlags_set(swigCPtr, this, value);
  }

  public int getStopFlags() {
    return libsumoJNI.TraCINextStopData_stopFlags_get(swigCPtr, this);
  }

  public void setDuration(double value) {
    libsumoJNI.TraCINextStopData_duration_set(swigCPtr, this, value);
  }

  public double getDuration() {
    return libsumoJNI.TraCINextStopData_duration_get(swigCPtr, this);
  }

  public void setUntil(double value) {
    libsumoJNI.TraCINextStopData_until_set(swigCPtr, this, value);
  }

  public double getUntil() {
    return libsumoJNI.TraCINextStopData_until_get(swigCPtr, this);
  }

  public TraCINextStopData() {
    this(libsumoJNI.new_TraCINextStopData(), true);
  }

}