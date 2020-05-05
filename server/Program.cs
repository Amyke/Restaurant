using communication_lib;
using MessagePack;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Runtime.Serialization;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;

namespace restaurant_server
{
    class Program
    {
        static async Task Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var comm = new Communication(new IPEndPoint(IPAddress.Any, 9007));
            CancellationTokenSource tokenSource = new CancellationTokenSource();
            CancellationToken _cancellation = tokenSource.Token;
            Model model = new Model();
            await using (var connectionHandler = new ConnectionHandler(comm, model, _cancellation))
            {
                Console.CancelKeyPress += (s, args) =>
                {
                    tokenSource.Cancel();
                    args.Cancel = true;
                };
                try
                {
                    Console.WriteLine("Listening...");
                    await comm.Listen(_cancellation);
                }
                catch(OperationCanceledException)
                {
                    Console.WriteLine("Listen finished");
                }
            }

            Console.WriteLine("Goodbye cruel world");
        }
    }
}
