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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class TcpTracer extends Tracer{

    InputStream in;
    boolean close;
    
    TcpTracer(int port) {
        super();
        try {
            final ServerSocket server = new ServerSocket(port);
            new Thread(new Runnable() {

                @Override
                public void run() {
                    handleServer(server);
                }
            }) {

            }.start();
        } catch (IOException e) {
        }
    }

    private void handleServer(ServerSocket server) {
        while (!close) {
            try {
                Socket socket = server.accept();
                OutputStream out = socket.getOutputStream();
                in = socket.getInputStream(); // dummy input to find out connection state
                setOutput(out);                
                while (in.read() != -1)
                    ;
                setOutput(null);
                if (in != null)
                    try {
                        in.close();
                    } catch (IOException e) {
                    }
                if (out != null)
                    try {
                        out.close();
                    } catch (IOException e) {
                    }
                in = null;
            } catch (Throwable e) {
            }
        }

    }
    
    protected void close() {
        super.close();
        if (in != null)
            try {
                in.close();
            } catch (IOException e) {
            }     
        close =true;
    }
}
