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

#include<map>
#include<string>
#include <systemc.h>
#include <stdio.h>
#include "impulse_sc.h"


static const char* _merge(const char* n1, const char* n2,char* buffer) {
	sprintf(buffer, "%s/%s", n1, n2);
	return buffer;
}

static std::map <std::string, int> handles;
/*******************************************************************************
 API
 *******************************************************************************/

int IMPNM(AddSignalSc)(sc_module* module, const char* name, int process,
		int type, const char* descriptor) {

	// merge module + name
	if  (module != NULL){
		char mname [1024];
		_merge(module->basename(), name,mname);
		name = mname;
	}
	const char* base;
	static struct BaseTable {
		double resolution;
		const char* base;
	} baseTable[] = { { sc_time(1, SC_FS).to_seconds(), "fs" }, { sc_time(10,
			SC_FS).to_seconds(), "fs10" }, { sc_time(100, SC_FS).to_seconds(),
			"fs100" }, { sc_time(1, SC_PS).to_seconds(), "ps" }, { sc_time(10,
			SC_PS).to_seconds(), "ps10" }, { sc_time(100, SC_PS).to_seconds(),
			"ps100" }, { sc_time(1, SC_NS).to_seconds(), "ns" }, { sc_time(10,
			SC_NS).to_seconds(), "ns10" }, { sc_time(100, SC_NS).to_seconds(),
			"ns100" }, { sc_time(1, SC_US).to_seconds(), "us" }, { sc_time(10,
			SC_US).to_seconds(), "us10" }, { sc_time(100, SC_US).to_seconds(),
			"us100" }, { sc_time(1, SC_MS).to_seconds(), "ms" }, { sc_time(10,
			SC_MS).to_seconds(), "ms10" }, { sc_time(100, SC_MS).to_seconds(),
			"ms100" }, { sc_time(1, SC_SEC).to_seconds(), "sec" }, { sc_time(10,
			SC_SEC).to_seconds(), "sec10" }, {
			sc_time(100, SC_SEC).to_seconds(), "100 sec" } };
	static int count = sizeof(baseTable) / sizeof(BaseTable);
	double resolution = sc_get_time_resolution().to_seconds();
	for (int i = 0; i < count; i++) {
		if (resolution == baseTable[i].resolution) {
			base = baseTable[i].base;
			break;
		}
	}

	return IMPNM(AddSignal)(name, process, type,
			descriptor, base);
}
int IMPNM(AssertSignalSc)(sc_module* module, const char* name, int process,
		int type, const char* descriptor) {

	// merge module + name
	if  (module != NULL){
		char mname [1024];
		_merge(module->basename(), name,mname);
		name = mname;
	}
	std::map<std::string,int>::iterator it;
	std::string id;
	id = id + name;
	it = handles.find(id);
	if (it != handles.end()){
		return it->second;
	}
	int handle =  IMPNM(AddSignalSc)(NULL,name, process, type,
				descriptor);
	handles[id]= handle;
	return handle;
}
void IMPNM_F(OpenSignal,Sc)(int handle){
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	IMPNM(OpenSignal)(handle, units,0);
}
void IMPNM_F(OpenAllSignals,Sc)(){
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	IMPNM(OpenAllSignals)(units,0);
}
void IMPNM_F(CloseSignal,Sc)(int handle){
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	IMPNM(CloseSignal)(handle, units);
}
void IMPNM_F(CloseAllSignals,Sc)(){
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	IMPNM(CloseAllSignals)(units);
}

void IMPNM2F(WriteInt,64,Sc)(int handle, boolean conflict, long long value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,64)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,32,Sc)(int handle, boolean conflict, long value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,32)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,16,Sc)(int handle, boolean conflict, short value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,16)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,8,Sc)(int handle, boolean conflict, char value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,8)(handle, (long long) units, conflict, value);
}
void IMPNM_F(WriteInt,Sc)(int handle, boolean conflict, int value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM(WriteInt)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,U64,Sc)(int handle, boolean conflict,
		unsigned long long value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,U64)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,U32,Sc)(int handle, boolean conflict,
		unsigned long value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,U32)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,U16,Sc)(int handle, boolean conflict,
		unsigned short value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,U16)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,U8,Sc)(int handle, boolean conflict,
		unsigned char value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,U8)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteInt,U,Sc)(int handle, boolean conflict,
		unsigned int value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteInt,U)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteFloat,32,Sc)(int handle, boolean conflict, float value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteFloat,32)(handle, (long long) units, conflict, value);
}
void IMPNM2F(WriteFloat,64,Sc)(int handle, boolean conflict, double value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM2(WriteFloat,64)(handle, (long long) units, conflict, value);
}
void IMPNM_F(WriteText,Sc)(int handle, boolean conflict, const char* value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM(WriteText)(handle, (long long) units, conflict, value);
}
void IMPNM_F(WriteEvent,Sc)(int handle, boolean conflict, int value) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM(WriteEvent)(handle, (long long) units, conflict, value);
}
void IMPNM_F(WriteBinary,Sc)(int handle, boolean conflict, const unsigned char* value,unsigned int length) {
	double resolution = sc_get_time_resolution().to_seconds();
	double units = sc_time_stamp().to_seconds() / resolution;
	return IMPNM(WriteBinary)(handle, (long long) units, conflict, value,length);
}

