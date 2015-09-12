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

package de.toem.basics.trace;

import java.io.IOException;
import java.io.OutputStream;

public class OutputStreamTracer extends Tracer {

    OutputStream out;

    OutputStreamTracer(OutputStream out) {
        super();
        setOutput(out);

    }

    protected void close() {
        super.close();
        if (out != null)
            try {
                out.close();
            } catch (IOException e) {
            }
    }
}
