using communication_lib.Implementation;
using MessagePack;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;

namespace communication_lib
{

    class ProtocolWriter
    {
        BufferBlock<Message> _messageQueue = new BufferBlock<Message>();
        Stream _stream;

        public ProtocolWriter(Stream stream)
        {
            _stream = stream;
        }

        public async Task WriteMessage(Message msg, CancellationToken cancellation)
        {
            await MessagePackSerializer.SerializeAsync(_stream, new MessageHeader { Id = msg.Id }, cancellationToken: cancellation);
            await MessagePackSerializer.SerializeAsync(msg.GetType(), _stream, msg, cancellationToken: cancellation);
        }
    }

    class Protocol
    {
        TcpClient _tcpClient;
        Client _client;
        ProtocolWriter _writer;
        ProtocolReader _reader;
        bool _running;

        public IClient Client => _client;

        public Protocol(TcpClient client)
        {
            _tcpClient = client;
            _client = new Client(this, _tcpClient.Client.RemoteEndPoint.ToString());
            _writer = new ProtocolWriter(_tcpClient.GetStream());
            _reader = new ProtocolReader(_tcpClient.GetStream(), _client);
        }

        public async Task Process(CancellationToken cancellation)
        {
            _running = true;
            await Task.Yield();
            try
            {
                while (true)
                {
                    await _reader.ProcessMessages(cancellation);
                    if (!_tcpClient.Client.Poll(0, SelectMode.SelectError))
                    {
                        return;
                    }
                }
            }
            catch (Exception ex) when (ex is ObjectDisposedException || ex is IOException)
            {
                _ = ex;
            }
            finally
            {
                _running = false;
                _client.OnConnectionLost();
            }
        }

        public async Task SendMessage(Message msg, CancellationToken cancellation)
        {
            await _writer.WriteMessage(msg, cancellation);
        }

        internal async Task Close()
        {
            _tcpClient.Close();
            while (_running)
            {
                await Task.Yield();
            }
        }
    }

    public class Communication : ICommunication
    {
        private IPEndPoint _endpoint;
        private TcpListener _listener;
        private List<Protocol> _acceptedClients = new List<Protocol>();

        public IPEndPoint ListenAddress => _endpoint;

        public event EventHandler<IClient>? ConnectionEstablished;

        public Communication(IPEndPoint address)
        {
            _endpoint = address;
            _listener = new TcpListener(_endpoint);
        }

        public async Task Listen(CancellationToken cancellation)
        {
            _listener.Start();
            using (cancellation.Register(() =>
            {
                _listener.Stop();
            }))
            {
                _endpoint = (IPEndPoint)_listener.LocalEndpoint;
                try
                {
                    while (!cancellation.IsCancellationRequested)
                    {
                        await Task.Yield();
                        var protocol = new Protocol(await _listener.AcceptTcpClientAsync());
                        _acceptedClients.Add(protocol);
                        OnConnectionEstablished(protocol.Client);
                        _ = protocol.Process(cancellation).ConfigureAwait(false);
                    }
                }
                catch (Exception)
                {
                    cancellation.ThrowIfCancellationRequested();
                    throw;
                }
            }
        }

        public async Task<IClient> Connect(IPEndPoint address, CancellationToken cancellation)
        {
            var tcpClient = new TcpClient();
            await tcpClient.ConnectAsync(address.Address, address.Port);
            var protocol = new Protocol(tcpClient);
            _ = protocol.Process(cancellation).ConfigureAwait(false);
            return protocol.Client;
        }

        public void Close()
        {
            _listener.Stop();
        }

        internal void OnConnectionEstablished(IClient client)
        {
            ConnectionEstablished?.Invoke(this, client);
        }
    }
}
