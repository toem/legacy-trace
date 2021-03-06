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

import de.toem.basics.trace.Impulse.Process;
import de.toem.basics.trace.Impulse.DomainBase;
import de.toem.basics.trace.Impulse.Type;

public class Example2 {

    public static void main(String[] args) {
        
        // open tcp port
        Impulse.open(55668);
        // wait for somebody to connect - not required
        while (!Impulse.isConnected())
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
            }

        // define signals
        Object s1 = Impulse.addSignal("Integer/Integer1", Process.Discrete, Type.Integer, null, DomainBase.ns);
        Object s2 = Impulse.addSignal("Integer/Integer2", Process.Discrete, Type.Integer, null, DomainBase.ns);
        Object s3 = Impulse.addSignal("Integer/Integer3", Process.Discrete, Type.Integer, null, DomainBase.ns);
        Object s4 = Impulse.addSignal("Integer/Integer4", Process.Discrete, Type.Integer, null, DomainBase.ns);
        Object s5 = Impulse.addSignal("Float/Float1", Process.Discrete, Type.Float, null, DomainBase.ns);
        Object s6 = Impulse.addSignal("Float/Float2", Process.Discrete, Type.Float, null, DomainBase.ns);
        Object s7 = Impulse.addSignal("Text/Text1", Process.Discrete, Type.Text, null, DomainBase.ns);
        Object s8 = Impulse.addSignal("Event/Event1", Process.Discrete, Type.Event, null, DomainBase.ns);

        // trace
        for (int n = 0; n < 1000; n++) {
            long current = n * 10;
            Impulse.writeInt(s1, current, n == 100, n - 1000);
            Impulse.writeInt(s2, current, n == 100, -n);
            Impulse.writeInt(s3, current, n == 100, n * 3 - 100);
            Impulse.writeInt(s4, current, n == 100, (char) n);
            Impulse.writeFloat(s5, current, n == 100, (float) Math.sin(n / 23.0));
            Impulse.writeFloat(s6, current, n == 100, (double) Math.cos(n / 55.0) * 10);
            if ((n % 100) == 0)
                Impulse.writeText(s7, current, n == 100, "text");
            Impulse.writeEvent(s8, current, n == 100, n % 8);
        }
        Impulse.closeAllSignals(10000);

        // close server
        Impulse.close();
    }

}
