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



#ifndef _IMPULSE
#define _IMPULSE

/*******************************************************************************
	Types and definitions
 *******************************************************************************/

typedef unsigned char boolean;
typedef unsigned char byte;


#ifndef __cplusplus
#define IMPNM( name) Ip ## name
#define IMPNM2( name,post) Ip ## name ## post
#define IMPNM2F( name,post,flav) Ip ## name ## post ## flav
#define IMPNM_F( name,flav) Ip ## name ## flav
#else
#define IMPNM( name) Ip ## name
#define IMPNM2( name,post) Ip ## name
#define IMPNM2F( name,post,flav) Ip ## name ## flav
#define IMPNM_F( name,flav) Ip ## name ## flav
#endif


extern const int IMPNM(UnknownProcess);
extern const int IMPNM(Discrete);
extern const int IMPNM(Continuous);

extern const int IMPNM(UnknownType);
extern const int IMPNM(Event);
extern const int IMPNM(Integer);
extern const int IMPNM(Logic);
extern const int IMPNM(Float);
extern const int IMPNM(Text);
extern const int IMPNM(Binary);
extern const int IMPNM(Struct);


extern const int IMPNM(NoOutput);
extern const int IMPNM(DefaultOutput);
extern const int IMPNM(Error);
extern const int IMPNM(Warning);
extern const int IMPNM(Info);

/*******************************************************************************
	API
 *******************************************************************************/

int IMPNM(AddSignal)(const char* path, int process,int type,const char* descriptor,const char* base);

void IMPNM(OpenSignal)(int handle, long long units, long long rate);
void IMPNM(OpenAllSignals)(long long units, long long rate);
void IMPNM(CloseSignal)(int handle, long long units);
void IMPNM(CloseAllSignals)(long long units);

void IMPNM2(WriteInt,64)(int handle, long long units, boolean conflict, long long value);
void IMPNM2(WriteInt,32)(int handle, long long units, boolean conflict, long value);
void IMPNM2(WriteInt,16)(int handle, long long units, boolean conflict, short value);
void IMPNM2(WriteInt,8)(int handle, long long units, boolean conflict, char value);
void IMPNM(WriteInt)(int handle, long long units, boolean conflict, int value);
void IMPNM2(WriteInt,U64)(int handle, long long units, boolean conflict, unsigned long long value);
void IMPNM2(WriteInt,U32)(int handle, long long units, boolean conflict, unsigned long value);
void IMPNM2(WriteInt,U16)(int handle, long long units, boolean conflict, unsigned short value);
void IMPNM2(WriteInt,U8)(int handle, long long units, boolean conflict, unsigned char value);
void IMPNM2(WriteInt,U)(int handle, long long units, boolean conflict, unsigned int value);
void IMPNM2(WriteFloat,32)(int handle, long long units, boolean conflict, float value);
void IMPNM2(WriteFloat,64)(int handle, long long units, boolean conflict, double value);
void IMPNM(WriteText)(int handle, long long units, boolean conflict, const char* value);
void IMPNM(WriteEvent)(int handle, long long units, boolean conflict, int value);
void IMPNM(WriteBinary)(int handle, long long units, boolean conflict, const unsigned char* value, unsigned int length);
void IMPNM(SetOutput)(int out);

/*******************************************************************************
	MACROS
 *******************************************************************************/

#define IP_STATIC_ADD_DISC(name,type,descriptor,base) static int name = IMPNM(AddSignal)(#name,IMPNM(Discrete),type,#descriptor,#base);


#endif


