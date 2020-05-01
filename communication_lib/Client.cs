using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace communication_lib
{
    internal class Client : IClient
    {
        private string _address;
        Protocol _protocol;

        public string Address => _address;

        public Client(Protocol protocol, string address)
        {
            _protocol = protocol;
            _address = address;
        }

        public event EventHandler? ConnectionLost;

        public event EventHandler<Message>? MessageArrived;

        public async Task Send(Message message, CancellationToken cancellation)
        {
            await _protocol.SendMessage(message, cancellation);
        }

        public async Task Close()
        {
            await _protocol.Close();
        }

        internal void OnConnectionLost()
        {
            ConnectionLost?.Invoke(this, null);
        }

        internal void OnMessageArrived(Message msg)
        {
            MessageArrived?.Invoke(this, msg);
        }

        public async ValueTask DisposeAsync()
        {
            await Close();
        }
    }
}
