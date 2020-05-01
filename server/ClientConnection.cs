using communication_lib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace restaurant_server
{

    /*class ClientConnection
    {
        readonly TcpClient _client;
        readonly NetworkStream _stream;
        readonly IDictionary<MessageId, Func<Reader, Message>> _messages;

        public ClientConnection(TcpClient client, IDictionary<MessageId, Func<Reader, Message>> messages)
        {
            _client = client;
            _stream = _client.GetStream();
            _messages = messages;

            Console.WriteLine("Client accepted: {0}", client.Client.LocalEndPoint.ToString());
        }

        public async Task ProcessMessages()
        {
            await Task.Yield();
            while (true)
            {
                byte[] buffer = new byte[1024];

                await _stream.ReadAsync(buffer, 0, 8);
                var headerReader = new Reader(buffer, 8);
                MessageId messageId = headerReader.GetEnum<MessageId>();
                UInt32 messageSize = headerReader.GetUInt32();

                await _stream.ReadAsync(buffer, 0, (int)messageSize);
                var bodyReader = new Reader(buffer, (int)messageSize);
                if (_messages.TryGetValue(messageId, out var factory))
                {
                    var message = factory.Invoke(bodyReader);
                    var reply = HandleLoginMessage(message);
                    if (reply != null)
                    {
                        Console.WriteLine("Handled {0}, replying with {1}", message.GetType().Name, reply.GetType().Name);

                        var writer = new Writer();
                        writer.Put(reply);
                        var writeBuffer = writer.GetBuffer();
                        await _stream.WriteAsync(writeBuffer, 0, writeBuffer.Length);
                    }
                    else
                    {
                        Console.WriteLine("Couldn't handle {0}", message.GetType().Name);
                    }
                }
                else
                {
                    Console.WriteLine("Received unknown message {0}", messageId);
                }
            }
        }
        Message HandleLoginMessage(Message msg)
        {
            switch (msg)
            {
                case LoginRequestMessage request:
                    return HandleLoginMessage(request);
            }
            return null;
        }

        Message HandleLoginMessage(LoginRequestMessage msg)
        {
            if (msg.Name == msg.Password)
            {
                return new LoginReplyMessage { Status = LoginStatus.Ok };
            }
            else
            {
                return new LoginReplyMessage { Status = LoginStatus.Error };
            }
        }
    }*/
}