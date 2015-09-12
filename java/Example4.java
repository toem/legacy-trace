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

public class Example4 {

    public static void main(String [ ] args)
    {
        // open file
        Impulse.open("test.trace");       
        
        // trace
        for (int n=0;n<1000;n++){
            long current = n * 10;
            Impulse.writeInt("Integer/Integer1",current,n==100,n-1000);
            Impulse.writeInt("Integer/Integer2",current,n==100,-n);
            Impulse.writeInt("Integer/Integer3",current,n==100,n*3-100);
            Impulse.writeInt("Integer/Integer4",current,n==100,(char)n);
            Impulse.writeFloat("Float/Float1",current,n==100,(float)Math.sin(n/23.0));
            Impulse.writeFloat("Float/Float2",current,n==100,(double)Math.cos(n/55.0)*10);
                if ((n%100)==0)
                    Impulse.writeText("Text/Text1",current,n==100,"text");
                Impulse.writeEvent("Event/Event1",current,n==100,n%8);
        }
        Impulse.closeAllSignals(10000);

        // close server
        Impulse.close();
    }
    
}
