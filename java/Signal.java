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

import java.io.UnsupportedEncodingException;

import de.toem.basics.trace.Impulse.Command;
import de.toem.basics.trace.Impulse.DomainBase;
import de.toem.basics.trace.Impulse.Process;
import de.toem.basics.trace.Impulse.Type;

class Signal {

    // tracer
    Tracer tracer;

    // signal definition
    String path;
    Process process;
    Type type;
    String descriptor;
    DomainBase base;

    // creation
    long current;
    Object initialized;
    int id;

    public Signal(Tracer tracer, String path, Process process, Type type, String signalDescriptor, DomainBase base) {
        this.path =  path != null ? path.replaceAll("\\.", "\\/") : "unamed";
        this.process = process;
        this.type = type ;
        this.descriptor = signalDescriptor != null ? signalDescriptor : "";
        this.base = base;
        this.id = tracer.generateId();
    }

    // ----------------------
    // init
    //

    public boolean isInitialized(Object output) {
        return this.initialized == output;
    }

    public int initialize(Object output, byte[] buffer, int pos) {
        int start = pos;
        pos += writePlusVal( 0,buffer, pos);
        pos += writePlusVal( Command.Initialize.ordinal(),buffer, pos);
        pos += writePlusVal( id+1,buffer, pos);
        pos += writeStringVal(path,buffer, pos);
        pos += writePlusVal( process.ordinal(),buffer, pos);
        pos += writePlusVal( type.ordinal(),buffer, pos);
        pos += writeStringVal( descriptor,buffer, pos);
        pos += writeStringVal(String.valueOf(base),buffer, pos);
        pos += writeStringVal( "",buffer, pos); // optional
        this.initialized = output;
        return pos - start;
    }

    public int open(long units,long rate,int mode, int limitation, byte[] buffer, int pos) {
        current = units;
        int start = pos;
        pos += writePlusVal( 0,buffer, pos);
        pos += writePlusVal( Command.Open.ordinal(),buffer, pos);
        pos += writePlusVal( id+1,buffer, pos);
        pos += writeLongVal( units,buffer, pos);
        pos += writeLongVal( 0,buffer, pos); 
        pos += writePlusVal( mode,buffer, pos);
        pos += writePlusVal( limitation,buffer, pos);
        return pos - start;
    }
    
    public int close(long units, byte[] buffer, int pos) {
        current = units;
        int start = pos;
        pos += writePlusVal( 0,buffer, pos);
        pos += writePlusVal( Command.Close.ordinal(),buffer, pos);
        pos += writePlusVal( id+1,buffer, pos);
        pos += writeLongVal( units,buffer, pos);
        return pos - start;
    }
    
    // ----------------------
    // write
    //

    private int writeHead(long units,boolean conflict, byte[] buffer, int pos) {
        long delta = units - current;
        current = units;
        if (delta >= 0) {
            int start = pos;
            int first = (id+1) << 1;
            boolean extendedHeader = conflict; // 3 status bits (id,R,R,C,1) else (id,0)
            if (extendedHeader)
                first = (id+1) << 4 | 1 | (conflict?2:0);
            pos += writePlusVal(first,buffer, pos);
            pos += writePlusVal(delta,buffer, pos);
            return pos-start;
        }
        return 0;
    }  

    public int writeInt(long units, boolean conflict, long value, byte[] buffer, int pos) {
        int start = pos;
        pos += writeHead(units,conflict, buffer, pos);
        pos += writeLongVal(value, buffer, pos);
        return pos-start;
    }

    public int writeText(long units, boolean conflict, String value, byte[] buffer, int pos) {
        int start = pos;
        pos += writeHead(units,conflict, buffer, pos);
        pos += writeStringVal(value, buffer, pos);
        return pos-start;
    }

    public int writeFloat(long units, boolean conflict, float value, byte[] buffer, int pos) {
        int start = pos;
        pos += writeHead(units,conflict, buffer, pos);
        pos += writeFloatVal(value, buffer, pos);
        return pos-start;
    }

    public int writeFloat(long units, boolean conflict, double value, byte[] buffer, int pos) {
        int start = pos;
        pos += writeHead(units,conflict, buffer, pos);
        pos += writeDoubleVal(value, buffer, pos);
        return pos-start;
    }
    
    public int writeEvent(long units, boolean conflict, byte[] buffer, int pos) {
        int start = pos;
        pos += writeHead(units,conflict, buffer, pos);
        pos += writePlusVal(0, buffer, pos);
        return pos-start;
    }
    
    public int writeEvent(long units, boolean conflict,int value, byte[] buffer, int pos) {
        int start = pos;
        pos += writeHead(units,conflict, buffer, pos);
        pos += writeLongVal(value, buffer, pos);
        return pos-start;
    }
    
    // ----------------------
    // helpers
    //

    // read write of positive integers
    // PLUS data
    private static final int MASK_PLUS = 0x80; // 1 bit
    private static final int MASK_PLUS_DATA = 0x7f; // 7 bits
    private static final int DEFAULT_PLUS_LENGTH = 0x7; // 7 bits

    private int writePlusVal(long val,byte[] buffer, int pos) {
        int len = 1;
        while (true) {
            if (val <= MASK_PLUS_DATA) {
                buffer[pos++] = (byte) (val & MASK_PLUS_DATA);
                return len;
            }
            buffer[pos++] = (byte) ((val & MASK_PLUS_DATA) | MASK_PLUS);
            val >>>= DEFAULT_PLUS_LENGTH;
            len++;
        }
    }

    private int writeStringVal(String value,byte[] buffer, int pos) {
        try {
            byte[] bytes = value.getBytes("UTF-8");
            int buffered = writePlusVal(bytes.length,buffer, pos);
            System.arraycopy(bytes, 0, buffer, pos + buffered, bytes.length);
            buffered += bytes.length;
            return buffered;

        } catch (UnsupportedEncodingException e) {
        }
        return 0;
    }

    private int writeLongVal( long value,byte[] buffer, int pos) {

        int buffered = 1 + pos;
        int dlength = 0;
        // write data
        if (value == 0) {
            // NO DATA
        } else if (value > 0) {
            while (value != 0) {
                buffer[buffered++] = (byte) (value & 0xff);
                dlength++;
                value >>>= 8;
            }
            if ((buffer[buffered - 1] & 0x80) != 0) {
                buffer[buffered++] = (byte) 0x00;
                dlength++;
            }
        } else {
            while (value != -1) {
                buffer[buffered++] = (byte) (value & 0xff);
                dlength++;
                value >>= 8;
            }
            if ((buffer[buffered - 1] & 0x80) == 0) {
                buffer[buffered++] = (byte) 0xff;
                dlength++;
            }
        }
        buffer[pos] = (byte) dlength;
        return dlength + 1;
    }

    private int writeFloatVal(float value,byte[] buffer, int pos) {

        int buffered = pos + writePlusVal( 4,buffer, pos);
        int intdata = Float.floatToIntBits(value);
        buffer[buffered++] = (byte) (intdata & 0xff);
        intdata >>>= 8;
        buffer[buffered++] = (byte) (intdata & 0xff);
        intdata >>>= 8;
        buffer[buffered++] = (byte) (intdata & 0xff);
        intdata >>>= 8;
        buffer[buffered++] = (byte) (intdata & 0xff);

        return 5;
    }

    private int writeDoubleVal( double value,byte[] buffer, int pos) {

        int buffered = pos + writePlusVal( 8,buffer, pos);
        long longdata = Double.doubleToLongBits(value);
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);
        longdata >>>= 8;
        buffer[buffered++] = (byte) (longdata & 0xff);

        return 9;
    }

}
