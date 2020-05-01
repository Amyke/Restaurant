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
    /*
        [Serializable]
        struct Asd
        {
            public string Prop { get; set; }
        }

        class BFormatter : IFormatter
        {
            public SerializationBinder Binder { get; set; }
            public StreamingContext Context { get; set; }
            public ISurrogateSelector SurrogateSelector { get; set; }
            private MessageFactory _message = new MessageFactory();
            public byte[] buffer = new byte[1024];

            public object Deserialize(Stream serializationStream)
            //0001002200020006Gulyás0004Sajt
            { 
                //serializationStream.Read();
                throw new NotImplementedException();
            }

            public void Serialize(Stream serializationStream, object graph)
            {
                if (graph is UInt32)
                {
                    Console.WriteLine("Serializing: {0}", (UInt32)graph);
                    serializationStream.Write(BitConverter.GetBytes((UInt32)graph));
                }
                else if (graph is string)
                {
                    var encoded = Encoding.UTF8.GetBytes((string)graph);
                    Console.WriteLine("Serializing: {0}", (string)graph);
                    Serialize(serializationStream, (UInt32)encoded.Length);
                    serializationStream.Write(encoded);
                }
                else
                {
                    var members = FormatterServices.GetSerializableMembers(graph.GetType());
                    foreach(var member in FormatterServices.GetObjectData(graph, members))
                    {
                        Serialize(serializationStream, member);
                    }
                }
            }
        }
    */
    static class Extensions
    {
        public class MessageQueue
        {
            BufferBlock<Message> _queue = new BufferBlock<Message>();

            public MessageQueue(IClient client)
            {
                client.MessageArrived += async (s, msg) =>
                {
                    await _queue.SendAsync(msg);
                };
            }

            public Message Receive(TimeSpan? timeout = null)
            {
                return _queue.Receive(timeout ?? TimeSpan.FromMilliseconds(500));
            }
        }

        public static MessageQueue Messages(this IClient client)
        {
            return new MessageQueue(client);
        }
    }

    class Program
    {
        /*
        public static void SetUp(Communication comm, List<IClient> _connectedCliens)
        {
            comm.ConnectionEstablished += (s, client) =>
            {
                _connectedCliens.Add(client);
                client.ConnectionLost += (s, _) =>
                {
                    _connectedCliens.Remove(client);
                };
            };

        }
        public static void SendReplyMessage(List<IClient> _connectedCliens, MessageFactory msgfact, CancellationToken cancellation)
        {
            foreach (IClient client in _connectedCliens)
            {
                var msg = client.Messages();

                var arrived = msg.Receive();

                client.Send(msgfact.AnsverMessage(arrived), cancellation);
            }
        }*/

        static async Task Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            List<IClient> _connectedClients = new List<IClient>();
            Dictionary<IClient, bool> loginedClient = new Dictionary<IClient, bool>();
            bool admin = false;
            CancellationTokenSource tokenSource = new CancellationTokenSource();
            CancellationToken _cancellation = tokenSource.Token;
            var comm = new Communication(new IPEndPoint(IPAddress.Any, 9004));
            comm.ConnectionEstablished += (s, client) =>
            {
                _connectedClients.Add(client);
                EventHandler<Message> handleMessage = async (s, msg) => await client.Send(msg, _cancellation);
                client.MessageArrived += handleMessage;
                EventHandler handleConnectionLost = (s, _) =>
                {
                    client.MessageArrived -= handleMessage;
                    _connectedClients.Remove(client);
                };
                client.ConnectionLost += handleConnectionLost;
            };
            Console.WriteLine("Listening...");


            Console.CancelKeyPress += (s, _) =>
            {
                tokenSource.Cancel();
            };
            await comm.Listen(_cancellation);


            /*
            var obj = new Asd { Prop = "Hello" };
            var formatter = new BFormatter();
            using (var stream = new FileStream("binary.dat", FileMode.OpenOrCreate))
            {
                formatter.Serialize(stream, obj);
            }*/
        }
    }
}
