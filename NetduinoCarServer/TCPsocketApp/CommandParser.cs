using System;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.Netduino;
using System.IO.Ports;

namespace TCPsocketApp
{
    class CommandParser
    {
        private OutputPort led;
        static SerialPort transmitter = new SerialPort("COM1", 9600, Parity.None, 8, StopBits.One);
        public CommandParser(OutputPort led)
        {
            this.led = led;
            transmitter.Open();
        }

        public string Parse(string command)
        {
            string returnvalue;
            switch (command)
            {
                case "left":
                    left_func();
                    returnvalue = "left";
                    break;
                case "right":
                    right_func();
                    returnvalue = "right";                
                    break;
                case "go":
                    go_func();
                    returnvalue = "go";
                    break;
                case "back":
                    back_func();
                    returnvalue = "back";
                    break;
                case "stop":
                    stop_func();
                    returnvalue = "stop";
                    break;
                case "manual":
                    manual_func();
                    returnvalue = "manual";
                    break;
                case "followline":
                    followline_func();
                    returnvalue = "followline";
                    break;
                default:
                    returnvalue = "Invalid command, type \"h\" for help.";
                    break;
            }
            return returnvalue;
        }

        public string go_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x00, 0x01, 0x00, 0xff };
            transmitter.Write(bytData,0,bytData.Length);
            return "success";
        }

        public string back_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x00, 0x02, 0x00, 0xff };
            transmitter.Write(bytData, 0, bytData.Length);
            return "success";

        }

        public string stop_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x00, 0x00, 0x00, 0xff };
            transmitter.Write(bytData, 0, bytData.Length);
            return "success";
        }

        public string left_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x00, 0x03, 0x00, 0xff };
            transmitter.Write(bytData, 0, bytData.Length);
            return "success";
        }

        public string right_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x00, 0x04, 0x00, 0xff };
            transmitter.Write(bytData, 0, bytData.Length);
            return "success";
        }

        public string followline_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x13, 0x02, 0x00, 0xff };
            transmitter.Write(bytData, 0, bytData.Length);
            return "success";
        }
        public string manual_func()
        {
            byte[] bytData = new byte[] { 0xff, 0x13, 0x00, 0x00, 0xff };
            transmitter.Write(bytData, 0, bytData.Length);
            return "success";
        }
    }
}
