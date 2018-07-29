using System;
using Microsoft.SPOT;
using System.Net.Sockets;
using Microsoft.SPOT.Hardware;
using System.Threading;

namespace TCPsocketApp
{
    class StartListener
    {
        private Socket socket;
        private OutputPort led;
        private Thread listenThread;

        public StartListener(Socket socket, OutputPort led)
        {
            this.socket = socket;
            this.led = led;
        }

        public void Start()
        {
            ListenForClients lfc = new ListenForClients(socket, led);
            this.listenThread = new Thread(new ThreadStart(lfc.ThreadProc));
            this.listenThread.Start();
        }
    }
}
