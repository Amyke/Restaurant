using communication_lib;
using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Threading.Tasks;
using System.Threading;
using Microsoft.Extensions.Logging;

namespace restaurant_server
{
    internal class ConnectionHandler : IAsyncDisposable, IConnectionHandler
    {
        readonly ICommunication _communication;
        readonly CancellationToken _cancellation;
        private readonly IModel _model;
        private readonly ILogger<ConnectionHandler> _logger;

        readonly List<PendingClient> _pendingClients = new List<PendingClient>();
        readonly List<AdminClient> _adminClients = new List<AdminClient>();
        readonly List<CustomerClient> _customerClients = new List<CustomerClient>();

        public ConnectionHandler(ICommunication communication, IModel model, CancellationToken cancellation, ILogger<ConnectionHandler> logger)
        {
            _communication = communication;
            _cancellation = cancellation;
            _model = model;
            _logger = logger;
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
        public IEnumerable<string> GetLoggedInCustomers()
        {
            return _customerClients.Select(x => x.Name);
        }

        private void OnConnectionEstablished(object? sender, IClient e)
        {
            _logger.LogDebug("Client connected: {}", e.Address);

            e.MessageArrived += OnMessageArrived;
            e.ConnectionLost += OnConnectionLost;
            _pendingClients.Add(new PendingClient(e, _model, this));
        }

        public async void OnMessageArrived(object? sender, Message e)
        {
            _logger.LogDebug("Message arrived: {}", e.Id);
            await Task.Factory.StartNew(
                () => OnMessageArrivedAsync(sender, e),
                _cancellation,
                TaskCreationOptions.None,
                TaskScheduler.Default);
        }

        private async Task OnMessageArrivedAsync(object? sender, Message e)
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
                _logger.LogError("No such client: {}", client.Address);
            }
        }

        public async Task BroadcastToAdmins(Message msg)
        {
            _logger.LogDebug("BroadcastToAdmins: {}", msg.Id);
            await Task.WhenAll(_adminClients.Select(c => c.IClient.Send(msg, _cancellation)));
        }
        public async Task BrodcastToCustomers(Message msg)
        {
            _logger.LogDebug("BroadcastCustomers: {}", msg.Id);
            await Task.WhenAll(_customerClients.Select(c => c.IClient.Send(msg, _cancellation)));
        }

        public async Task SendToCustomer(string tableId, Message msg)
        {
            var client = _customerClients.SingleOrDefault(x => x.Name == tableId);
            if(client == null)
            {
                _logger.LogError("Client is not connected: {}", tableId);
                return;
            }
            await client.IClient.Send(msg, _cancellation);
        }

        private void OnConnectionLost(object? sender, EventArgs e)
        {
            if (sender is IClient client)
            {
                _logger.LogDebug("Connection lost: {}", client.Address);

                client.MessageArrived -= OnMessageArrived;
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
