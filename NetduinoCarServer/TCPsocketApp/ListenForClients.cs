using System;
using Microsoft.SPOT;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;
using Microsoft.SPOT.Hardware;

namespace TCPsocketApp
{
    class ListenForClients
    {
        private Socket socket;
        private OutputPort led;
        private Thread ClientHandlerThread;

        public ListenForClients(Socket m_socket, OutputPort m_led)
        {
            socket = m_socket;
            led = m_led;
        }

        public void ThreadProc()
        {
            while (true)
            {
                /*
                using (Socket clientSocket = socket.Accept())
                {
                    IPEndPoint clientIP = clientSocket.RemoteEndPoint as IPEndPoint;
                    EndPoint clientEndPoint = clientSocket.RemoteEndPoint;
                    clientSocket.ReceiveTimeout = 3000;
                    led.Write(true);
                    Thread.Sleep(150);
                    led.Write(false);

                    HandleClients hc = new HandleClients(clientSocket, led, clientIP, clientEndPoint);
                    this.ClientHandlerThread = new Thread(new ThreadStart(hc.ThreadProc));
                    this.ClientHandlerThread.Start();
                }
                */

                Socket clientSocket = socket.Accept();
                HandleClients hc = new HandleClients(clientSocket, led);
                this.ClientHandlerThread = new Thread(new ThreadStart(hc.ThreadProc));
                this.ClientHandlerThread.Start();
            }
        }
    }
}
