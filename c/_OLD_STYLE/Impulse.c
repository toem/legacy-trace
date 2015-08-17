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

#include "Impulse.h"

/*******************************************************************************
 Configuration
 *******************************************************************************/
#ifndef IMPULSE_MAX_SIGNALS
#define IMPULSE_MAX_SIGNALS 50
#endif
#ifndef IMPULSE_BUFFER_SIZE
#define IMPULSE_BUFFER_SIZE 4096
#endif
#ifndef IMPULSE_TRACER_BITS
#define IMPULSE_TRACER_BITS 0
#endif
#ifndef IMPULSE_TRACER_ID
#define IMPULSE_TRACER_ID 0
#endif

/*******************************************************************************
 Types
 *******************************************************************************/
enum Command {
	Initialize,Open,Close
};

/*******************************************************************************
 Internal data
 *******************************************************************************/
const char* SYNC = "de.toem.impulse.trace\x01";
typedef struct {
	// signal definition
	const char* path;
	Process process;
	Type type;
	const char* descriptor;
	const char* base;
	// creation
	long long current;
	int initialized;
	int id;
} Signal;
Signal signals[IMPULSE_MAX_SIGNALS];
int output = 0;
int count = 0;
byte buffer[IMPULSE_BUFFER_SIZE];

const int IMPULSE_NAME(NO_OUTPUT) =  0;
const int IMPULSE_NAME(DEFAULT_OUTPUT) =  1;
const int IMPULSE_NAME(NORMAL) =  0;
const int IMPULSE_NAME(CONFLICT) =  1;
const int IMPULSE_NAME(ERROR)=2;
const int IMPULSE_NAME(WARNING)=1;
const int IMPULSE_NAME(INFO)=0;

/*******************************************************************************
 Output prototype
 *******************************************************************************/

void writeOutput(int output, byte* buffer, int pos, int length);

/*******************************************************************************
 Basic Library
 *******************************************************************************/

int _strlen(const char *string) {
	int length = 0;
	while (*string != '\0') {
		string++;
		length++;
	};
	return length;
}

void _arraycopy(byte* source, int sPos, byte* target, int tPos, int length) {
	int n;
	for (n = 0; n < length; n++)
		target[tPos + n] = source[sPos + n];
}

/*******************************************************************************
 Low level writing
 *******************************************************************************/

// read write of positive integers
// PLUS data
const byte MASK_PLUS = 0x80; // 1 bit
const byte MASK_PLUS_DATA = 0x7f; // 7 bits
const byte DEFAULT_PLUS_LENGTH = 0x7; // 7 bits

int _writePlusVal(int val, byte* buffer, int pos) {
	int len = 1;
	while (1) {
		if (val <= MASK_PLUS_DATA) {
			buffer[pos++] = (byte)(val & MASK_PLUS_DATA);
			return len;
		}
		buffer[pos++] = (byte)((val & MASK_PLUS_DATA) | MASK_PLUS);
		val >>= DEFAULT_PLUS_LENGTH;
		len++;
	}
}

int _writeStringVal(const char* value, byte* buffer, int pos) {
	int buffered = _writePlusVal(_strlen(value), buffer, pos);
	_arraycopy((byte*) value, 0, buffer, pos + buffered, _strlen(value));
	buffered += _strlen(value);
	return buffered;
}

int _writeInt64Val(long long value, byte* buffer, int pos) {

	int buffered = 1 + pos;
	int dlength = 0;
	// write data
	if (value == 0) {
		// NO DATA
	} else if (value > 0) {
		while (value != 0) {
			buffer[buffered++] = (byte)(value & 0xff);
			dlength++;
			value >>= 8;
		}
		if ((buffer[buffered - 1] & 0x80) != 0) {
			buffer[buffered++] = (byte) 0x00;
			dlength++;
		}
	} else {
		while (value != -1) {
			buffer[buffered++] = (byte)(value & 0xff);
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

int _writeInt32Val(long value, byte* buffer, int pos) {

	int buffered = 1 + pos;
	int dlength = 0;
	// write data
	if (value == 0) {
		// NO DATA
	} else if (value > 0) {
		while (value != 0) {
			buffer[buffered++] = (byte)(value & 0xff);
			dlength++;
			value >>= 8;
		}
		if ((buffer[buffered - 1] & 0x80) != 0) {
			buffer[buffered++] = (byte) 0x00;
			dlength++;
		}
	} else {
		while (value != -1) {
			buffer[buffered++] = (byte)(value & 0xff);
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

int _writeIntU64Val(unsigned long long value, byte* buffer, int pos) {

	int buffered = 1 + pos;
	int dlength = 0;
	// write data
	if (value == 0) {
		// NO DATA
	} else {
		while (value != 0) {
			buffer[buffered++] = (byte)(value & 0xff);
			dlength++;
			value >>= 8;
		}
		if ((buffer[buffered - 1] & 0x80) != 0) {
			buffer[buffered++] = (byte) 0x00;
			dlength++;
		}
	}
	buffer[pos] = (byte) dlength;
	return dlength + 1;
}
int _writeIntU32Val(unsigned long value, byte* buffer, int pos) {

	int buffered = 1 + pos;
	int dlength = 0;
	// write data
	if (value == 0) {
		// NO DATA
	} else {
		while (value != 0) {
			buffer[buffered++] = (byte)(value & 0xff);
			dlength++;
			value >>= 8;
		}
		if ((buffer[buffered - 1] & 0x80) != 0) {
			buffer[buffered++] = (byte) 0x00;
			dlength++;
		}
	}
	buffer[pos] = (byte) dlength;
	return dlength + 1;
}

int _writeFloat32Val(float value, byte* buffer, int pos) {

	int buffered = pos + _writePlusVal(4, buffer, pos);
	long intdata = *(unsigned long*) &value;
	buffer[buffered++] = (byte)(intdata & 0xff);
	intdata >>= 8;
	buffer[buffered++] = (byte)(intdata & 0xff);
	intdata >>= 8;
	buffer[buffered++] = (byte)(intdata & 0xff);
	intdata >>= 8;
	buffer[buffered++] = (byte)(intdata & 0xff);

	return 5;
}

int _writeFloat64Val(double value, byte* buffer, int pos) {

	int buffered = pos + _writePlusVal(8, buffer, pos);
	unsigned long long longdata = *(unsigned long long *) &value;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);
	longdata >>= 8;
	buffer[buffered++] = (byte)(longdata & 0xff);

	return 9;
}

/*******************************************************************************
 Signal level writing
 *******************************************************************************/

int _initialize(Signal *signal, int output, byte* buffer,
		int pos) {
	int start = pos;
	pos += _writePlusVal(0, buffer, pos);
	pos += _writePlusVal(Initialize, buffer, pos);
	pos += _writePlusVal(signal->id + 1, buffer, pos);
	pos += _writeStringVal(signal->path, buffer, pos);
	pos += _writePlusVal(signal->process, buffer, pos);
	pos += _writePlusVal(signal->type, buffer, pos);
	pos += _writeStringVal(signal->descriptor, buffer, pos);
	pos += _writeStringVal(signal->base, buffer, pos);
	pos += _writeStringVal("", buffer, pos); // optional
	signal->initialized = output;
	return pos - start;
}

int _open(Signal *signal, long long units, long long rate,long mode, long limitation,byte* buffer,
		int pos) {
	signal->current = units;
	int start = pos;
	pos += _writePlusVal(0, buffer, pos);
	pos += _writePlusVal(Open, buffer, pos);
	pos += _writePlusVal(signal->id + 1, buffer, pos);
	pos += _writeInt64Val(units, buffer, pos);
	pos += _writeInt64Val(rate, buffer, pos);
	pos += _writePlusVal(mode, buffer, pos);
	pos += _writePlusVal(limitation, buffer, pos);
	return pos - start;
}

int _close(Signal *signal, long long units,  byte* buffer,
		int pos) {
	signal->current = units;
	int start = pos;
	pos += _writePlusVal(0, buffer, pos);
	pos += _writePlusVal(Close, buffer, pos);
	pos += _writePlusVal(signal->id + 1, buffer, pos);
	pos += _writeInt64Val(units, buffer, pos);
	return pos - start;
}

int _writeHead(Signal *signal, long long units, boolean conflict, byte* buffer,
		int pos) {
	long long delta = units - signal->current;
	signal->current = units;
	if (delta >= 0) {
		int start = pos;
		int first = (signal->id + 1) << 1;
		boolean extendedHeader = conflict; // 3 status bits (id,R,R,C,1) else (id,0)
		if (extendedHeader)
			first = (signal->id + 1) << 4 | 1 | (conflict ? 2 : 0);
		pos += _writePlusVal(first, buffer, pos);
		pos += _writePlusVal(delta, buffer, pos);
		return pos - start;
	}
	return 0;
}

int _writeInt64Signal(Signal *signal, long long units, boolean conflict,
		long long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeInt64Val(value, buffer, pos);
	return pos - start;
}

int _writeInt32Signal(Signal *signal, long long units, boolean conflict,
		 long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeInt32Val(value, buffer, pos);
	return pos - start;
}

int _writeIntU64Signal(Signal *signal, long long units, boolean conflict,
		 unsigned long long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeIntU64Val(value, buffer, pos);
	return pos - start;
}

int _writeIntU32Signal(Signal *signal, long long units, boolean conflict,
		 unsigned long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeIntU32Val(value, buffer, pos);
	return pos - start;
}
int _writeFloat32Signal(Signal *signal, long long units, boolean conflict,
		float value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeFloat32Val(value, buffer, pos);
	return pos - start;
}

int _writeFloat64Signal(Signal *signal, long long units, boolean conflict,
		double value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeFloat64Val(value, buffer, pos);
	return pos - start;
}

int _writeTextSignal(Signal *signal, long long units, boolean conflict,
		const char* value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeStringVal(value, buffer, pos);
	return pos - start;
}

int _writeEventSignal(Signal *signal, long long units, boolean conflict,
		 int value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeInt32Val(value, buffer, pos);
	return pos - start;
}

/*******************************************************************************
 API
 *******************************************************************************/

void IMPULSE_NAME(setOutput)(int out) {
	output = out;
	if (out != NO_OUTPUT)
		writeOutput(out,(byte*)SYNC,0,_strlen(SYNC));
	
}

int IMPULSE_NAME(addSignal)(const char* path, Process process, Type type,
		const char* descriptor, const char* base) {
	signals[count].path = path != 0 ? path : "unamed";
	signals[count].process = process;
	signals[count].type = type;
	signals[count].descriptor = descriptor != 0 ? descriptor : "";
	signals[count].base = base;
	int id  = count++;
	signals[id].id = id;
	return id;
}

void IMPULSE_NAME(openSignal)(int handle, long long units, long long rate) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0) {
		int pos = 0;
		pos += _open(signal,units,rate,0,0, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}

void IMPULSE_NAME(openAllSignals)(long long units, long long rate) {
	int n=0;
	for(n=0;n<count;n++)
		IMPULSE_NAME(openSignal)(n,units,rate);	
}

void IMPULSE_NAME(closeSignal)(int handle, long long units) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0) {
		int pos = 0;
		pos += _close(signal,units, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}	
}

void IMPULSE_NAME(closeAllSignals)(long long units) {
	int n=0;
	for(n=0;n<count;n++)
		IMPULSE_NAME(closeSignal)(n,units);	
}

void IMPULSE_NAME(writeInt64)(int handle, long long units, boolean conflict, long long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Integer && signal->process == Discrete) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeInt64Signal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}

void IMPULSE_NAME(writeInt32)(int handle, long long units, boolean conflict, long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Integer && signal->process == Discrete) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeInt32Signal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}
void IMPULSE_NAME(writeInt16)(int handle, long long units, boolean conflict, short value) {
	writeInt32(handle,units,conflict,value);
}
void IMPULSE_NAME(writeInt8)(int handle, long long units, boolean conflict, char value) {
	writeInt32(handle,units,conflict,value);
}
void IMPULSE_NAME(writeIntU64)(int handle, long long units, boolean conflict, unsigned long long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Integer && signal->process == Discrete) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeIntU64Signal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}

void IMPULSE_NAME(writeIntU32)(int handle, long long units, boolean conflict, unsigned long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Integer && signal->process == Discrete) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeIntU32Signal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}
void IMPULSE_NAME(writeIntU16)(int handle, long long units, boolean conflict, unsigned short value) {
	writeIntU32(handle,units,conflict,value);
}
void IMPULSE_NAME(writeIntU8)(int handle, long long units, boolean conflict, unsigned char value) {
	writeIntU32(handle,units,conflict,value);
}

void IMPULSE_NAME(writeFloat32)(int handle, long long units, boolean conflict, float value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Float && (signal->process == Discrete || signal->process == Continuous)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeFloat32Signal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}

void IMPULSE_NAME(writeFloat64)(int handle, long long units, boolean conflict, double value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Float && (signal->process == Discrete || signal->process == Continuous)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeFloat64Signal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}

void IMPULSE_NAME(writeText)(int handle, long long units, boolean conflict, const char* value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Text && signal->process == Discrete) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeTextSignal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}

void IMPULSE_NAME(writeEvent)(int handle, long long units, boolean conflict, int value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == Event && signal->process == Discrete) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeEventSignal(signal, units, conflict, value, buffer, pos);
		writeOutput(output, buffer, 0, pos);
	}
}
