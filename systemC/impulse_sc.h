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



#ifndef _IMPULSE_SC
#define _IMPULSE_SC

#include <systemc.h>
#include "impulse.h"

/*******************************************************************************
	Types and definitions
 *******************************************************************************/

/*******************************************************************************
	API
 *******************************************************************************/

int IMPNM(AddSignalSc)(sc_module* module,const char* name, int process,int type,const char* descriptor);
int IMPNM(AssertSignalSc)(sc_module* module,const char* name, int process,int type,const char* descriptor);

void IMPNM_F(OpenSignal,Sc)(int handle);
void IMPNM_F(OpenAllSignals,Sc)();
void IMPNM_F(CloseSignal,Sc)();
void IMPNM_F(CloseAllSignals,Sc)();

void IMPNM2F(WriteInt,64,Sc)(int handle, boolean conflict, long long value);
void IMPNM2F(WriteInt,32,Sc)(int handle, boolean conflict, long value);
void IMPNM2F(WriteInt,16,Sc)(int handle, boolean conflict, short value);
void IMPNM2F(WriteInt,8,Sc)(int handle, boolean conflict, char value);
void IMPNM_F(WriteInt,Sc)(int handle, boolean conflict, int value);
void IMPNM2F(WriteInt,U64,Sc)(int handle, boolean conflict, unsigned long long value);
void IMPNM2F(WriteInt,U32,Sc)(int handle, boolean conflict, unsigned long value);
void IMPNM2F(WriteInt,U16,Sc)(int handle, boolean conflict, unsigned short value);
void IMPNM2F(WriteInt,U8,Sc)(int handle, boolean conflict, unsigned char value);
void IMPNM2F(WriteInt,U,Sc)(int handle, boolean conflict, unsigned int value);
void IMPNM2F(WriteFloat,32,Sc)(int handle, boolean conflict, float value);
void IMPNM2F(WriteFloat,64,Sc)(int handle, boolean conflict, double value);
void IMPNM_F(WriteText,Sc)(int handle, boolean conflict, const char* value);
void IMPNM_F(WriteEvent,Sc)(int handle, boolean conflict, int value);
void IMPNM_F(WriteBinary,Sc)(int handle, boolean conflict, const unsigned char* value,unsigned int length);

/*******************************************************************************
	MACROS
 *******************************************************************************/


// module == this
#define IP_STAT_WRITE_DISC_INT_SC(name,descriptor,conflict,value)   static int name = IMPNM(AddSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Integer),#descriptor);IpWriteIntSc(name, conflict, value);
#define IP_STAT_WRITE_DISC_FLOAT_SC(name,descriptor,conflict,value) static int name = IMPNM(AddSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Float),#descriptor);IpWriteFloatSc(name, conflict, value);
#define IP_STAT_WRITE_DISC_EVENT_SC(name,descriptor,conflict,value) static int name = IMPNM(AddSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Event),#descriptor);IpWriteEventSc(name, conflict, value);
#define IP_STAT_WRITE_DISC_BINARY_SC(name,descriptor,conflict,value, length) static int name = IMPNM(AddSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Binary),#descriptor);IpWriteBinarySc(name, conflict, value,length);

// explicit module
#define IP_STAT_WRITE_DISC_INT_SCM(module,name,descriptor,conflict,value)   static int name = IMPNM(AddSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Integer),#descriptor);IpWriteIntSc(name, conflict, value);
#define IP_STAT_WRITE_DISC_FLOAT_SCM(module,name,descriptor,conflict,value) static int name = IMPNM(AddSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Float),#descriptor);IpWriteFloatSc(name, conflict, value);
#define IP_STAT_WRITE_DISC_EVENT_SCM(module,name,descriptor,conflict,value) static int name = IMPNM(AddSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Event),#descriptor);IpWriteEventSc(name, conflict, value);
#define IP_STAT_WRITE_DISC_BINARY_SCM(module,name,descriptor,conflict,value, length) static int name = IMPNM(AddSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Binary),#descriptor);IpWriteBinarySc(name, conflict, value,length);

// using global handles table
#define IP_TABL_WRITE_DISC_INT_SC(name,descriptor,conflict,value)    IpWriteIntSc(IMPNM(AssertSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Integer),#descriptor), conflict, value);
#define IP_TABL_WRITE_DISC_FLOAT_SC(name,descriptor,conflict,value)  IpWriteFloatSc(IMPNM(AssertSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Float),#descriptor), conflict, value);
#define IP_TABL_WRITE_DISC_EVENT_SC(name,descriptor,conflict,value)  IpWriteEventSc(IMPNM(AssertSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Event),#descriptor), conflict, value);
#define IP_TABL_WRITE_DISC_BINARY_SC(name,descriptor,conflict,value, length)  IpWriteBinarySc(IMPNM(AssertSignalSc)(this,#name,IMPNM(Discrete),IMPNM(Binary),#descriptor), conflict, value,length);

#define IP_TABL_WRITE_DISC_INT_SCM(module,name,descriptor,conflict,value)    IpWriteIntSc(IMPNM(AssertSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Integer),#descriptor), conflict, value);
#define IP_TABL_WRITE_DISC_FLOAT_SCM(module,name,descriptor,conflict,value)  IpWriteFloatSc(IMPNM(AssertSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Float),#descriptor), conflict, value);
#define IP_TABL_WRITE_DISC_EVENT_SCM(module,name,descriptor,conflict,value)  IpWriteEventSc(IMPNM(AssertSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Event),#descriptor), conflict, value);
#define IP_TABL_WRITE_DISC_BINARY_SCM(module,name,descriptor,conflict,value, length)  IpWriteBinarySc(IMPNM(AssertSignalSc)(module,#name,IMPNM(Discrete),IMPNM(Binary),#descriptor), conflict, value,length);


#endif
