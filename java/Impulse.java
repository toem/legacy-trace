/*******************************************************************************
 * Copyright (c) 2012-2015  toem and others
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

import java.io.OutputStream;

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
        ys, ys10, ys100, zs, zs10, zs100, as, as10, as100, fs, fs10, fs100, ps, ps10, ps100, ns, ns10, ns100, us, us10, us100, ms, ms10, ms100, s, s10, s100, ks, ks10, ks100, Ms, Ms10, Ms100, Gs, Gs10, Gs100, Ts, Ts10, Ts100, Ps, Ps10, Ps100, Es, Es10, Es100, Zs, Zs10, Zs100, Ys, Index,
        yHz, yHz10, yHz100, zHz, zHz10, zHz100, aHz, aHz10, aHz100, fHz, fHz10, fHz100, pHz, pHz10, pHz100, nHz, nHz10, nHz100, uHz, uHz10, uHz100, mHz, mHz10, mHz100, Hz, Hz10, Hz100, kHz, kHz10, kHz100, MHz, MHz10, MHz100, GHz, GHz10, GHz100, THz, THz10, THz100, PHz, PHz10, PHz100, EHz, EHz10, EHz100, ZHz, ZHz10, ZHz100, YHz,
        yA, yA10, yA100, zA, zA10, zA100, aA, aA10, aA100, fA, fA10, fA100, pA, pA10, pA100, nA, nA10, nA100, uA, uA10, uA100, mA, mA10, mA100, A, A10, A100, kA, kA10, kA100, MA, MA10, MA100, GA, GA10, GA100, TA, TA10, TA100, PA, PA10, PA100, EA, EA10, EA100, ZA, ZA10, ZA100, YA,
        yX, yX10, yX100, zX, zX10, zX100, aX, aX10, aX100, fX, fX10, fX100, pX, pX10, pX100, nX, nX10, nX100, uX, uX10, uX100, mX, mX10, mX100, X, X10, X100, kX, kX10, kX100, MX, MX10, MX100, GX, GX10, GX100, TX, TX10, TX100, PX, PX10, PX100, EX, EX10, EX100, ZX, ZX10, ZX100, YX,
        yV, yV10, yV100, zV, zV10, zV100, aV, aV10, aV100, fV, fV10, fV100, pV, pV10, pV100, nV, nV10, nV100, uV, uV10, uV100, mV, mV10, mV100, V, V10, V100, kV, kV10, kV100, MV, MV10, MV100, GV, GV10, GV100, TV, TV10, TV100, PV, PV10, PV100, EV, EV10, EV100, ZV, ZV10, ZV100, YV;

    
    }

    // -------------------------------------------------------------------------
    // internal
    //

    private static Tracer tracer;

    protected enum Command {
        Initialize, Open, Close;
    }

    static final String SYNC = "de.toem.impulse.trace\001";

    // -------------------------------------------------------------------------
    // create
    //

    public static void open(int port) {
        tracer = new TcpTracer(port);
    }

    public static void open(String fileName) {
        tracer = new FileTracer(fileName);
    }

    public static void open(OutputStream out) {
        tracer = new OutputStreamTracer(out);
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

    public static Object addSignal(String path, Process process, Type type, String descriptor, DomainBase base) {
        if (tracer != null)
            return tracer.addSignal(path, process, type, descriptor, base);
        return null;
    }

    public static Object addSignal(String path, String process, String type, String descriptor, String base) {
        try {
            Process p = Process.valueOf(process);
            Type t = Type.valueOf(type);
            DomainBase b = DomainBase.valueOf(base);

            if (tracer != null)
                return tracer.addSignal(path, p, t, descriptor, b);
        } catch (Throwable e) {
            e.printStackTrace();
        }
        return null;
    }

    public static void openSignal(Object ref, long units, long rate) {
        if (tracer != null)
            tracer.openSignal(ref, units, rate);
    }

    public static void openAllSignals(long units, long rate) {
        if (tracer != null)
            tracer.openAllSignals(units, rate);
    }

    public static void closeSignal(Object ref, long units) {
        if (tracer != null)
            tracer.closeSignal(ref, units);
    }

    public static void closeAllSignals(long units) {
        if (tracer != null)
            tracer.closeAllSignals(units);
    }

    public static void writeInt(Object ref, long units, boolean conflict, int value) {
        if (tracer != null)
            tracer.writeInt(ref, units, conflict, value);
    }

    public static void writeInt(Object ref, long units, boolean conflict, long value) {
        if (tracer != null)
            tracer.writeInt(ref, units, conflict, value);
    }

    public static void writeText(Object ref, long units, boolean conflict, String value) {
        if (tracer != null)
            tracer.writeText(ref, units, conflict, value);
    }

    public static void writeFloat(Object ref, long units, boolean conflict, float value) {
        if (tracer != null)
            tracer.writeFloat(ref, units, conflict, value);
    }

    public static void writeFloat(Object ref, long units, boolean conflict, double value) {
        if (tracer != null)
            tracer.writeFloat(ref, units, conflict, value);
    }

    public static void writeEvent(Object ref, long units, boolean conflict, int value) {
        if (tracer != null)
            tracer.writeEvent(ref, units, conflict, value);
    }

    public static void setDefaultBase(DomainBase base) {
        if (tracer != null)
            tracer.defaultBase = base;
    }
    public static void setDefaultBase(String base) {
        try {
            DomainBase b = DomainBase.valueOf(base);

            if (tracer != null)
                tracer.defaultBase = b;
        } catch (Throwable e) {
            e.printStackTrace();
        } 

    }
    public static Process processForName(String name) {
        return Process.valueOf(name);
    }

    public static Type typeForName(String name) {
        return Type.valueOf(name);
    }

    public static DomainBase baseForName(String name) {
        return DomainBase.valueOf(name);
    }
}
