using System;
using ipcBridge.NET;

namespace DemoClient
{
    class Entrypoint
    {
        static void Main(string[] args)
        {
            var connection = BridgeServer.Connect("mar");
            Console.Write("Say something: ");
            string input = Console.ReadLine();

            var raw = System.Text.Encoding.ASCII.GetBytes(input);
            connection.Write(raw);

            connection.Signal();
            connection.Dispose();
            Console.WriteLine("Exiting");
        }
    }
}
