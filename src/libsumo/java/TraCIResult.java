/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.12
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.eclipse.sumo.libsumo;

public class TraCIResult {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected TraCIResult(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(TraCIResult obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        libsumoJNI.delete_TraCIResult(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public String getString() {
    return libsumoJNI.TraCIResult_getString(swigCPtr, this);
  }

  public TraCIResult() {
    this(libsumoJNI.new_TraCIResult(), true);
  }

}
