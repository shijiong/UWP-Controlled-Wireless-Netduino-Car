using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using System.IO.Ports;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;

namespace TCPsocketApp
{
    public class Program
    {
        private static EndPoint ep;
        private static int bl;
        private static OutputPort led = new OutputPort(Pins.ONBOARD_LED, false);
        private static Socket socket;

        public static void Main()
        {
            int port = 8080;

            Thread.Sleep(8000);

            Microsoft.SPOT.Net.NetworkInformation.NetworkInterface networkInterface = Microsoft.SPOT.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces()[0];

            Debug.Print("my ip address: " + networkInterface.IPAddress.ToString());

            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            //Request and bind to an IP from DHCP server
            socket.Bind(new IPEndPoint(IPAddress.Any, port));
            //Debug print our IP address
            Debug.Print(Microsoft.SPOT.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces()[0].IPAddress);
            //Start listen for web requests
            socket.Listen(10);
            Debug.Print("listening");
            StartListener listener = new StartListener(socket, led);
            listener.Start();

        }

        
    }
}
