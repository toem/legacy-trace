/*******************************************************************************
 * Copyright (c) 2012-2014  toem and others
 * All rights reserved. This source code and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     toem - initial API and implementation
 * More information under http://toem.de
 *******************************************************************************/

package de.toem.basics.trace;



public class Impulse {

    // -------------------------------------------------------------------------
    // Definitions
    //

   
    public enum Process {
       Unknown, Discrete, Continuous;
    }

    public enum Type {
        Unknown, Event, Integer, Logic, Float, Text, Binary, Struct;
    }

    public enum DomainBase {
        ys, ys10, ys100, zs, zs10, zs100, as, as10, as100, fs, fs10, fs100, ps, ps10, ps100, ns, ns10, ns100, us, us10, us100, ms, ms10, ms100, s, s10, s100, ks, ks10, ks100, Ms, Ms10, Ms100, Gs, Gs10, Gs100, Ts, Ts10, Ts100, Ps, Ps10, Ps100, Es, Es10, Es100, Zs, Zs10, Zs100, Ys,Index;
    }
    
    // -------------------------------------------------------------------------
    // internal
    //
    
    private static Tracer tracer;

    protected enum Command {
        Initialize,Open,Close;
    }

    static final String SYNC = "de.toem.impulse.trace\001";
    
    // -------------------------------------------------------------------------
    // create
    //

    public static void open(int port) {
        tracer = new TcpTracer(port);
    }
    
    public static boolean isConnected() {
        if (tracer != null)
            return tracer.isConnected();
        return false;
    }
    
    public static void close() {
        if (tracer != null)
            tracer.close();
    }
    // -------------------------------------------------------------------------
    // use
    //
  
    public static Object addSignal(String path, Process process,Type type,String descriptor,DomainBase base) {
        if (tracer != null)
            return tracer.addSignal(path, process, type,descriptor,base);
        return null;
    }
    
    public static void openSignal(Object ref, long units, long rate) {
        if (tracer != null)
            tracer.openSignal(ref, units, rate);
    }
    
    public static void openAllSignals(long units, long rate) {
        if (tracer != null)
            tracer.openAllSignals(units,rate);
    }
    
    public static void closeSignal(Object ref, long units) {
        if (tracer != null)
            tracer.closeSignal(ref, units);
    }
    
    public static void closeAllSignals(long units) {
        if (tracer != null)
            tracer.closeAllSignals(units);
    }

    public static void writeInt(Object ref, long units,boolean conflict,  int value) {
        if (tracer != null)
            tracer.writeInt(ref, units,conflict, value);
    }
    
    public static void writeInt(Object ref, long units,boolean conflict,  long value) {
        if (tracer != null)
            tracer.writeInt(ref, units,conflict, value);
    }

    public static void writeText(Object ref, long units,boolean conflict,  String value) {
        if (tracer != null)
            tracer.writeText(ref, units,conflict, value);
    }

    public static void writeFloat(Object ref, long units,boolean conflict,  float value) {
        if (tracer != null)
            tracer.writeFloat(ref, units,conflict, value);
    }
    
    public static void writeFloat(Object ref, long units,boolean conflict,  double value) {
        if (tracer != null)
            tracer.writeFloat(ref, units,conflict, value);
    }

    public static void writeEvent(Object ref, long units,boolean conflict,  int value) {
        if (tracer != null)
            tracer.writeEvent(ref, units,conflict, value);
    }
}
