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
 
#include<stdio.h>
#include<math.h>
#include "Impulse.h"
  
FILE *ptr_myfile;
int main( int argc, const char* argv[] )
{	
	int n = 0;
	ptr_myfile=fopen("test.trace","wb");	
	
	int h1 = addSignal("Integer/writeInt64",Discrete,Integer,0,"ns");
	int h2 = addSignal("Integer/writeInt32",Discrete,Integer,0,"ns");
	int h3 = addSignal("Integer/writeInt16",Discrete,Integer,0,"ns");
	int h4 = addSignal("Integer/writeInt8",Discrete,Integer,0,"ns");
	int h5 = addSignal("Integer/writeIntU64",Discrete,Integer,0,"ns");
	int h6 = addSignal("Integer/writeIntU32",Discrete,Integer,0,"ns");
	int h7 = addSignal("Integer/writeIntU16",Discrete,Integer,0,"ns");
	int h8 = addSignal("Integer/writeIntU8",Discrete,Integer,0,"ns");
	int h9 = addSignal("Float/writeFloat32",Discrete,Float,0,"ns");
	int h10 = addSignal("Float/writeFloat64",Discrete,Float,0,"ns");
	int h11 = addSignal("Text/writeText",Discrete,Text,0,"ns");
	int h12 = addSignal("Event/writeEvent",Discrete,Event,0,"ns");
	
	setOutput(DEFAULT_OUTPUT);
	
	for (n=0;n<1000;n++){
		long long current = n * 10;
		writeInt64(h1,current,n==100?CONFLICT:NORMAL,n-1000);
		writeInt32(h2,current,n==100?CONFLICT:NORMAL,-n);
		writeInt16(h3,current,n==100?CONFLICT:NORMAL,n*3-100);
		writeInt8(h4,current,n==100?CONFLICT:NORMAL,(char)n);
		writeIntU64(h5,current,n==100?CONFLICT:NORMAL,n);
		writeIntU32(h6,current,n==100?CONFLICT:NORMAL,n%100);
		writeIntU16(h7,current,n==100?CONFLICT:NORMAL,n);
		writeIntU8(h8,current,n==100?CONFLICT:NORMAL,n*8);
		writeFloat32(h9,current,n==100?CONFLICT:NORMAL,(float)sin(n/23.0));
		writeFloat64(h10,current,n==100?CONFLICT:NORMAL,(double)cos(n/55.0)*10);
			if (!(n%100))
		writeText(h11,current,n==100?CONFLICT:NORMAL,"text");
		writeEvent(h12,current,n==100?CONFLICT:NORMAL,n%8);
	}
	closeAllSignals(10000);
	fclose(ptr_myfile);
}


void writeOutput(int output, byte* buffer, int pos, int length) {
	fwrite(buffer+pos,length, 1, ptr_myfile);
}
