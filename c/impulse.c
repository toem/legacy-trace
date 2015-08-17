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

#include "impulse.h"

/*******************************************************************************
 Configuration
 *******************************************************************************/
#ifndef IMPULSE_MAX_SIGNALS
#define IMPULSE_MAX_SIGNALS 50
#endif
#ifndef IMPULSE_BUFFER_SIZE
#define IMPULSE_BUFFER_SIZE 65536
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
	int process;
	int type;
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

const int IMPNM(UnknownProcess) =  0;
const int IMPNM(Discrete) =  1;
const int IMPNM(Continuous) =  2;

const int IMPNM(UnknownType) =  0;
const int IMPNM(Event) =  1;
const int IMPNM(Integer) = 2;
const int IMPNM(Logic) =  3;
const int IMPNM(Float) =  4;
const int IMPNM(Text) =  5;
const int IMPNM(Binary) =  6;
const int IMPNM(Struct) =  7;

const int IMPNM(NoOutput) =  0;
const int IMPNM(DefaultOutput) =  1;
const int IMPNM(Error)=2;
const int IMPNM(Warning)=1;
const int IMPNM(Info)=0;

/*******************************************************************************
 Output prototype
 *******************************************************************************/

void IMPNM(WriteOutput)(int output, byte* buffer, int pos, int length);

/*******************************************************************************
 Basic Library
 *******************************************************************************/

static int _strlen(const char *string) {
	int length = 0;
	while (*string != '\0') {
		string++;
		length++;
	};
	return length;
}

static void _arraycopy(byte* source, int sPos, byte* target, int tPos, int length) {
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

static int _writePlusVal(int val, byte* buffer, int pos) {
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
	return 0;
}

static int _writeStringVal(const char* value, byte* buffer, int pos) {
	int buffered = _writePlusVal(_strlen(value), buffer, pos);
	_arraycopy((byte*) value, 0, buffer, pos + buffered, _strlen(value));
	buffered += _strlen(value);
	return buffered;
}

static int _writeBinaryVal(const unsigned char* value, unsigned int length, byte* buffer, int pos) {
	int buffered = _writePlusVal(length, buffer, pos);
	_arraycopy((byte*) value, 0, buffer, pos + buffered, length);
	buffered += length;
	return buffered;
}

static int _writeInt64Val(long long value, byte* buffer, int pos) {

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

static int _writeInt32Val(long value, byte* buffer, int pos) {

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

static int _writeIntU64Val(unsigned long long value, byte* buffer, int pos) {

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
static int _writeIntU32Val(unsigned long value, byte* buffer, int pos) {

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

static int _writeFloat32Val(float value, byte* buffer, int pos) {

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

static int _writeFloat64Val(double value, byte* buffer, int pos) {

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

static int _initialize(Signal *signal, int output, byte* buffer,
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

static int _open(Signal *signal, long long units, long long rate,long mode, long limitation,byte* buffer,
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

static int _close(Signal *signal, long long units,  byte* buffer,
		int pos) {
	signal->current = units;
	int start = pos;
	pos += _writePlusVal(0, buffer, pos);
	pos += _writePlusVal(Close, buffer, pos);
	pos += _writePlusVal(signal->id + 1, buffer, pos);
	pos += _writeInt64Val(units, buffer, pos);
	return pos - start;
}

static int _writeHead(Signal *signal, long long units, boolean conflict, byte* buffer,
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

static int _writeInt64Signal(Signal *signal, long long units, boolean conflict,
		long long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeInt64Val(value, buffer, pos);
	return pos - start;
}

static int _writeInt32Signal(Signal *signal, long long units, boolean conflict,
		 long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeInt32Val(value, buffer, pos);
	return pos - start;
}

static int _writeIntU64Signal(Signal *signal, long long units, boolean conflict,
		 unsigned long long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeIntU64Val(value, buffer, pos);
	return pos - start;
}

static int _writeIntU32Signal(Signal *signal, long long units, boolean conflict,
		 unsigned long value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeIntU32Val(value, buffer, pos);
	return pos - start;
}
static int _writeFloat32Signal(Signal *signal, long long units, boolean conflict,
		float value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeFloat32Val(value, buffer, pos);
	return pos - start;
}

static int _writeFloat64Signal(Signal *signal, long long units, boolean conflict,
		double value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeFloat64Val(value, buffer, pos);
	return pos - start;
}

static int _writeTextSignal(Signal *signal, long long units, boolean conflict,
		const char* value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeStringVal(value, buffer, pos);
	return pos - start;
}

static int _writeEventSignal(Signal *signal, long long units, boolean conflict,
		 int value, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeInt32Val(value, buffer, pos);
	return pos - start;
}

static int _writeBinarySignal(Signal *signal, long long units, boolean conflict,
		const unsigned char* value, unsigned int length, byte* buffer, int pos) {
	int start = pos;
	pos += _writeHead(signal, units, conflict, buffer, pos);
	pos += _writeBinaryVal(value,length, buffer, pos);
	return pos - start;
}

/*******************************************************************************
 API
 *******************************************************************************/

void IMPNM(SetOutput)(int out) {
	output = out;
	if (out != IMPNM(NoOutput))
		IMPNM(WriteOutput)(out,(byte*)SYNC,0,_strlen(SYNC));

}

int IMPNM(AddSignal)(const char* path, int process, int type,
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

void IMPNM(OpenSignal)(int handle, long long units, long long rate) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0) {
		int pos = 0;
		pos += _open(signal,units,rate,0,0, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM(OpenAllSignals)(long long units, long long rate) {
	int n=0;
	for(n=0;n<count;n++)
		IMPNM(OpenSignal)(n,units,rate);
}

void IMPNM(CloseSignal)(int handle, long long units) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0) {
		int pos = 0;
		pos += _close(signal,units, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM(CloseAllSignals)(long long units) {
	int n=0;
	for(n=0;n<count;n++)
		IMPNM(CloseSignal)(n,units);
}

void IMPNM2(WriteInt,64)(int handle, long long units, boolean conflict, long long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Integer) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeInt64Signal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM2(WriteInt,32)(int handle, long long units, boolean conflict, long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Integer) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeInt32Signal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM2(WriteInt,16)(int handle, long long units, boolean conflict, short value) {
	IMPNM2(WriteInt,32)(handle,units,conflict,(long)value);
}
void IMPNM2(WriteInt,8)(int handle, long long units, boolean conflict, char value) {
	IMPNM2(WriteInt,32)(handle,units,conflict,(long)value);
}
void IMPNM(WriteInt)(int handle, long long units, boolean conflict, int value) {
	IMPNM2(WriteInt,64)(handle,units,conflict,(long long)value);
}
void IMPNM2(WriteInt,U64)(int handle, long long units, boolean conflict, unsigned long long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Integer) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeIntU64Signal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM2(WriteInt,U32)(int handle, long long units, boolean conflict, unsigned long value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Integer) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeIntU32Signal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}
void IMPNM2(WriteInt,U16)(int handle, long long units, boolean conflict, unsigned short value) {
	IMPNM2(WriteInt,U32)(handle,units,conflict,(unsigned long)value);
}
void IMPNM2(WriteInt,U8)(int handle, long long units, boolean conflict, unsigned char value) {
	IMPNM2(WriteInt,U32)(handle,units,conflict,(unsigned long)value);
}
void IMPNM2(WriteInt,U)(int handle, long long units, boolean conflict, unsigned int value) {
	IMPNM2(WriteInt,U64)(handle,units,conflict,(unsigned long long)value);
}
void IMPNM2(WriteFloat,32)(int handle, long long units, boolean conflict, float value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Float) && (signal->process == IMPNM(Discrete) || signal->process == IMPNM(Continuous))) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeFloat32Signal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM2(WriteFloat,64)(int handle, long long units, boolean conflict, double value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Float) && (signal->process == IMPNM(Discrete) || signal->process == IMPNM(Continuous))) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeFloat64Signal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM(WriteText)(int handle, long long units, boolean conflict, const char* value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Text) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeTextSignal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM(WriteEvent)(int handle, long long units, boolean conflict, int value) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Event) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeEventSignal(signal, units, conflict, value, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

void IMPNM(WriteBinary)(int handle, long long units, boolean conflict, const unsigned char* value, unsigned int length) {
	if (handle < 0 || handle >= count)
		return;
	Signal *signal = &signals[handle];
	if (output != 0 && signal->type == IMPNM(Binary) && signal->process == IMPNM(Discrete)) {
		int pos = 0;
		if (signal->initialized != output){
			pos += _initialize(signal, output, buffer, pos);
			pos += _open(signal, units,0,0,0, buffer, pos);
		}
		pos += _writeBinarySignal(signal, units, conflict, value, length, buffer, pos);
		IMPNM(WriteOutput)(output, buffer, 0, pos);
	}
}

/*******************************************************************************
 helper
 *******************************************************************************/


