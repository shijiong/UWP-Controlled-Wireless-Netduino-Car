using System;
using Microsoft.SPOT;
using System.Net.Sockets;
using Microsoft.SPOT.Hardware;
using System.Net;
using System.Text;
using System.Threading;

namespace TCPsocketApp
{
    class HandleClients : IDisposable
    {
        private Socket clientSocket;
        private OutputPort led;
        private IPEndPoint clientIP;
        private EndPoint clientEndPoint;

        public HandleClients(Socket clientSocket, OutputPort led)
        {
            this.clientSocket = clientSocket;
            this.led = led;
            clientIP = clientSocket.RemoteEndPoint as IPEndPoint;
            clientEndPoint = clientSocket.RemoteEndPoint;
        }

        public void ThreadProc()
        {
            CommandParser parser = new CommandParser(led);

            while (true)
            {
                int bytesReceived = 0;
                while (bytesReceived == 0)
                {
                    bytesReceived = clientSocket.Available;
                }

                if (bytesReceived > 0)
                {
                    byte[] buffer = new byte[bytesReceived];
                    int byteCount = clientSocket.Receive(buffer, bytesReceived, SocketFlags.None);
                    string request = new string(Encoding.UTF8.GetChars(buffer));
                    Debug.Print(request);

                    string response = parser.Parse(request);

                    clientSocket.Send(Encoding.UTF8.GetBytes(response), response.Length, SocketFlags.None);

                    //led.Write(true);
                    //Thread.Sleep(150);
                    //led.Write(false);
                }
            }
        }

        #region IDisposable Members
        ~HandleClients()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (clientSocket != null)
                clientSocket.Close();
        }
        #endregion
    }
}
