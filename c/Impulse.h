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
 
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
	Types and definitions
 *******************************************************************************/
 
typedef unsigned char boolean;
typedef unsigned char byte;

#ifdef IMPULSE_PREFIX
#define IMPULSE_NAME( name) IMPULSE_PREFIX ## name
#else
#define IMPULSE_NAME( name) name
#endif

typedef enum {
    UnknownProcess=0, Discrete, Continuous
}IMPULSE_NAME(Process);

typedef enum {
    UnknownType=0, Event, Integer, Logic, Float, Text, Binary, Struct
}IMPULSE_NAME(Type);

extern const int IMPULSE_NAME(NO_OUTPUT);
extern const int IMPULSE_NAME(DEFAULT_OUTPUT);
extern const int IMPULSE_NAME(NORMAL);
extern const int IMPULSE_NAME(CONFLICT);
extern const int IMPULSE_NAME(ERROR);
extern const int IMPULSE_NAME(WARNING);
extern const int IMPULSE_NAME(INFO);

/*******************************************************************************
	API
 *******************************************************************************/
    
int IMPULSE_NAME(addSignal)(const char* path, Process process,Type type,const char* descriptor,const char* base);

void IMPULSE_NAME(openSignal)(int handle, long long units, long long rate);
void IMPULSE_NAME(openAllSignals)(long long units, long long rate);
void IMPULSE_NAME(closeSignal)(int handle, long long units);
void IMPULSE_NAME(closeAllSignals)(long long units);

void IMPULSE_NAME(writeInt64)(int handle, long long units, boolean conflict, long long value);
void IMPULSE_NAME(writeInt32)(int handle, long long units, boolean conflict, long value);
void IMPULSE_NAME(writeInt16)(int handle, long long units, boolean conflict, short value);
void IMPULSE_NAME(writeInt8)(int handle, long long units, boolean conflict, char value);
void IMPULSE_NAME(writeIntU64)(int handle, long long units, boolean conflict, unsigned long long value);
void IMPULSE_NAME(writeIntU32)(int handle, long long units, boolean conflict, unsigned long value);
void IMPULSE_NAME(writeIntU16)(int handle, long long units, boolean conflict, unsigned short value);
void IMPULSE_NAME(writeIntU8)(int handle, long long units, boolean conflict, unsigned char value);
void IMPULSE_NAME(writeFloat32)(int handle, long long units, boolean conflict, float value);
void IMPULSE_NAME(writeFloat64)(int handle, long long units, boolean conflict, double value);
void IMPULSE_NAME(writeText)(int handle, long long units, boolean conflict, const char* value);
void IMPULSE_NAME(writeEvent)(int handle, long long units, boolean conflict, int value);

#ifdef __cplusplus
}
#endif
    
    