using communication_lib;
using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Threading.Tasks;
using System.Threading;

namespace restaurant_server
{
    internal class ConnectionHandler : IAsyncDisposable, IConnectionHandler
    {
        readonly ICommunication _communication;
        readonly CancellationToken _cancellation;
        private readonly IModel _model;

        readonly List<PendingClient> _pendingClients = new List<PendingClient>();
        readonly List<AdminClient> _adminClients = new List<AdminClient>();
        readonly List<CustomerClient> _customerClients = new List<CustomerClient>();

        public ConnectionHandler(ICommunication communication, IModel model, CancellationToken cancellation)
        {
            _communication = communication;
            _cancellation = cancellation;
            _model = model;
            _communication.ConnectionEstablished += OnConnectionEstablished;
        }

        public void CreateAdmin(string name, IClient client)
        {
            var newAdmin = new AdminClient(client, name, _model, this);
            _adminClients.Add(newAdmin);
            _pendingClients.RemoveAll(c => c.IClient == client);
        }

        public void CreateCustomer(string name, IClient client)
        {
            var newCustomer = new CustomerClient(client, name, _model, this);
            _customerClients.Add(newCustomer);
            _pendingClients.RemoveAll(c => c.IClient == client);
        }

        private void OnConnectionEstablished(object? sender, IClient e)
        {
            e.MessageArrived += OnMessageArrivedAsync;
            e.ConnectionLost += OnConnectionLost;
            _pendingClients.Add(new PendingClient(e, _model, this));
        }

        public async void OnMessageArrivedAsync(object? sender, Message e)
        {
            IClient? client = sender as IClient;
            if (client == null)
            {
                // error
                return;
            }

            if (e.Id == MessageId.LoginRequest)
            {
                var pendingClient = _pendingClients.Find(c => c.IClient == client!);
                if (pendingClient == null)
                {
                    // error, either:
                    // * no such client
                    // * admin client
                    // * customer client
                    return;
                }
                var msg = (LoginRequestMessage)e;
                await pendingClient.LoginRequested(msg.Name, msg.Password, _cancellation);
                return;
            }

            if (_customerClients.FirstOrDefault(c => c.IClient == client!) is CustomerClient customer)
            {
                await customer.HandleMessage(e, _cancellation);
            }
            else if (_adminClients.FirstOrDefault(c => c.IClient == client!) is AdminClient admin)
            {
                await admin.HandleMessage(e, _cancellation);
            }
            else
            {
                // error
            }
        }

        public async Task BroadcastToAdmins(Message msg)
        {
            await Task.WhenAll(_adminClients.Select(c => c.IClient.Send(msg, _cancellation)));
        }

        private void OnConnectionLost(object? sender, EventArgs e)
        {
            if (sender is IClient client)
            {
                client.MessageArrived -= OnMessageArrivedAsync;
                client.ConnectionLost -= OnConnectionLost;

                _pendingClients.RemoveAll(c => c.IClient == client);
                _customerClients.RemoveAll(c => c.IClient == client);
                _adminClients.RemoveAll(c => c.IClient == client);
            }
        }

        public ValueTask DisposeAsync()
        {
            _communication.Close();
            return new ValueTask();
        }
    }
}
