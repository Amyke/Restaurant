﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using communication_lib;

namespace restaurant_server
{
    internal class PendingClient
    {
        private readonly IConnectionHandler _connectionHandler;
        private readonly IModel _model;

        public IClient IClient { get; internal set; }

        public PendingClient(IClient client, IModel model, IConnectionHandler connectionHandler)
        {
            _connectionHandler = connectionHandler;
            _model = model;
            IClient = client;
        }

        public async Task LoginRequested(string name, string password, CancellationToken cancellation)
        {
            switch (await _model.Login(name, password))
            {
                case LoginResult.Customer:
                    await IClient.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, cancellation);
                    _connectionHandler.CreateCustomer(name, IClient);
                    break;
                case LoginResult.Admin:
                    await IClient.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, cancellation);
                    _connectionHandler.CreateAdmin(name, IClient);
                    break;
                case LoginResult.Deny:
                    await IClient.Send(new LoginReplyMessage { Status = LoginStatus.Error }, cancellation);
                    break;
            }
        }
    }
}
