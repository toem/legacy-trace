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
 
#include<stdio.h>
#include<math.h>
#include "impulse.h"
  
FILE *ptr_myfile;
int main( int argc, const char* argv[] )
{	
	int n = 0;
	ptr_myfile=fopen("test.trace","wb");	
	
	int h1 = IpAddSignal("Integer/writeInt64",IpDiscrete,IpInteger,0,"ns");
	int h2 = IpAddSignal("Integer/writeInt32",IpDiscrete,IpInteger,0,"ns");
	int h3 = IpAddSignal("Integer/writeInt16",IpDiscrete,IpInteger,0,"ns");
	int h4 = IpAddSignal("Integer/writeInt8",IpDiscrete,IpInteger,0,"ns");
	int h5 = IpAddSignal("Integer/writeIntU64",IpDiscrete,IpInteger,0,"ns");
	int h6 = IpAddSignal("Integer/writeIntU32",IpDiscrete,IpInteger,0,"ns");
	int h7 = IpAddSignal("Integer/writeIntU16",IpDiscrete,IpInteger,0,"ns");
	int h8 = IpAddSignal("Integer/writeIntU8",IpDiscrete,IpInteger,0,"ns");
	int h9 = IpAddSignal("Float/writeFloat32",IpDiscrete,IpFloat,0,"ns");
	int h10 = IpAddSignal("Float/writeFloat64",IpDiscrete,IpFloat,0,"ns");
	int h11 = IpAddSignal("Text/writeText",IpDiscrete,IpText,0,"ns");
	int h12 = IpAddSignal("Event/writeEvent",IpDiscrete,IpEvent,0,"ns");
	
	IpSetOutput(IpDefaultOutput);
	
	for (n=0;n<1000;n++){
		long long current = n * 10;
		
	#ifndef __cplusplus
		IpWriteInt64(h1,current,n==100,n-1000);
		IpWriteInt32(h2,current,n==100,-n);
		IpWriteInt16(h3,current,n==100,n*3-100);
		IpWriteInt8(h4,current,n==100,(char)n);
		IpWriteIntU64(h5,current,n==100,n);
		IpWriteIntU32(h6,current,n==100,n%100);
		IpWriteIntU16(h7,current,n==100,n);
		IpWriteIntU8(h8,current,n==100,n*8);
		IpWriteFloat32(h9,current,n==100,(float)sin(n/23.0));
		IpWriteFloat64(h10,current,n==100,(double)cos(n/55.0)*10);
	#else
		IpWriteInt(h1,current,n==100,n-1000);
		IpWriteInt(h2,current,n==100,-n);
		IpWriteInt(h3,current,n==100,n*3-100);
		IpWriteInt(h4,current,n==100,(char)n);
		IpWriteInt(h5,current,n==100,n);
		IpWriteInt(h6,current,n==100,n%100);
		IpWriteInt(h7,current,n==100,n);
		IpWriteInt(h8,current,n==100,n*8);
		IpWriteFloat(h9,current,n==100,(float)sin(n/23.0));
		IpWriteFloat(h10,current,n==100,(double)cos(n/55.0)*10);
	#endif
		if (!(n%100))
			IpWriteText(h11,current,n==100,"text");
		IpWriteEvent(h12,current,n==100,n%8);
	}
	IpCloseAllSignals(10000);
	fclose(ptr_myfile);
}


void IpWriteOutput(int output, byte* buffer, int pos, int length) {
	fwrite(buffer+pos,length, 1, ptr_myfile);
}
