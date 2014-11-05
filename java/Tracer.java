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

import java.io.IOException;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

import de.toem.basics.trace.Impulse.DomainBase;
import de.toem.basics.trace.Impulse.Process;
import de.toem.basics.trace.Impulse.Type;

public class Tracer {

    // id generator
    public int nextId = 0;
    public int tracerIdBits = 0;
    public int tracerId = 0;

    public DomainBase defaultBase = DomainBase.ms;

    private byte[] buffer = new byte[4096];

    private OutputStream output;

    private Map<String, Signal> signals = new HashMap<String, Signal>();

    protected synchronized void setOutput(OutputStream output) {
        this.output = output;
        try {
            if (output != null)
                output.write(Impulse.SYNC.getBytes("UTF-8"));
        } catch (IOException e) {
        }
    }

    protected boolean isConnected() {
        return this.output != null;
    }

    protected void close() {
        setOutput(null);
    }

    protected synchronized Object addSignal(String path, Process process, Type type, String descriptor, DomainBase base) {
        Signal signal = null;
        signals.put(String.valueOf(path), signal = new Signal(this, path, process, type, descriptor, base));
        return signal;
    }

    protected synchronized void openSignal(Object ref, long units, long rate) {
        Signal signal = (Signal) (ref instanceof Signal ? ref : signals.get(String.valueOf(ref)));
        if (output != null && signal != null) {
            int pos = 0;
            pos += signal.open(units, rate, 0, 0, buffer, pos);
            try {
                output.write(buffer, 0, pos);
            } catch (IOException e) {
            }
        }
    }

    protected synchronized void openAllSignals(long units, long rate) {
        for (Signal signal : signals.values())
            if (signal.initialized != null)
                openSignal(signal, units,rate);
    }
    
    protected synchronized void closeSignal(Object ref, long units) {
        Signal signal = (Signal) (ref instanceof Signal ? ref : signals.get(String.valueOf(ref)));
        if (output != null && signal != null) {
            int pos = 0;
            pos += signal.close(units, buffer, pos);
            try {
                output.write(buffer, 0, pos);
            } catch (IOException e) {
            }
        }
    }

    protected synchronized void closeAllSignals(long units) {
        for (Signal signal : signals.values())
            if (signal.initialized != null)
                closeSignal(signal, units);
    }

    protected synchronized void writeInt(Object ref, long units, boolean conflict, long value) {
        Signal signal = (Signal) (ref instanceof Signal ? ref : signals.get(String.valueOf(ref)));
        if (signal == null)
            signals.put(String.valueOf(ref), signal = new Signal(this, String.valueOf(ref), Process.Discrete, Type.Integer, null, defaultBase));
        if (output != null && signal.type == Type.Integer && signal.process == Process.Discrete) {
            int pos = 0;
            if (!signal.isInitialized(output)) {
                pos += signal.initialize(output, buffer, pos);
                pos += signal.open(units, 0, 0, 0, buffer, pos);
            }
            pos += signal.writeInt(units, conflict, value, buffer, pos);
            try {
                output.write(buffer, 0, pos);
            } catch (IOException e) {
            }
        }
    }

    protected synchronized void writeText(Object ref, long units, boolean conflict, String value) {
        Signal signal = (Signal) (ref instanceof Signal ? ref : signals.get(String.valueOf(ref)));
        if (signal == null)
            signals.put(String.valueOf(ref), signal = new Signal(this, String.valueOf(ref), Process.Discrete, Type.Text, null, defaultBase));
        if (output != null && signal.type == Type.Text && signal.process == Process.Discrete) {
            int pos = 0;
            if (!signal.isInitialized(output)) {
                pos += signal.initialize(output, buffer, pos);
                pos += signal.open(units, 0, 0, 0, buffer, pos);
            }
            pos += signal.writeText(units, conflict, value, buffer, pos);
            try {
                output.write(buffer, 0, pos);
            } catch (IOException e) {
            }
        }
    }

    protected synchronized void writeFloat(Object ref, long units, boolean conflict, Object value) {
        Signal signal = (Signal) (ref instanceof Signal ? ref : signals.get(String.valueOf(ref)));
        if (signal == null)
            signals.put(String.valueOf(ref), signal = new Signal(this, String.valueOf(ref), Process.Discrete, Type.Float, null, defaultBase));
        if (output != null && signal.type == Type.Float && (signal.process == Process.Discrete || signal.process == Process.Continuous)) {
            int pos = 0;
            if (!signal.isInitialized(output)) {
                pos += signal.initialize(output, buffer, pos);
                pos += signal.open(units, 0, 0, 0, buffer, pos);
            }
            if (value instanceof Double)
                pos += signal.writeFloat(units, conflict, (Double) value, buffer, pos);
            else if (value instanceof Float)
                pos += signal.writeFloat(units, conflict, (Float) value, buffer, pos);
            else
                pos += signal.writeFloat(units, conflict, 0.0, buffer, pos);
            try {
                output.write(buffer, 0, pos);
            } catch (IOException e) {
            }
        }
    }

    protected synchronized void writeEvent(Object ref, long units, boolean conflict, Object value) {
        Signal signal = (Signal) (ref instanceof Signal ? ref : signals.get(String.valueOf(ref)));
        if (signal == null)
            signals.put(String.valueOf(ref), signal = new Signal(this, String.valueOf(ref), Process.Discrete, Type.Event, null, defaultBase));
        if (output != null && signal.type == Type.Event && signal.process == Process.Discrete) {
            int pos = 0;
            if (!signal.isInitialized(output)) {
                pos += signal.initialize(output, buffer, pos);
                pos += signal.open(units, 0, 0, 0, buffer, pos);
            }
            if (value instanceof Number)
                pos += signal.writeEvent(units, conflict, ((Number) value).intValue(), buffer, pos);
            else
                pos += signal.writeEvent(units, conflict, buffer, pos);
            try {
                output.write(buffer, 0, pos);
            } catch (IOException e) {
            }
        }
    }

    public int generateId() {
        int id = nextId << tracerIdBits | tracerId;
        nextId++;
        return id;
    }
}
