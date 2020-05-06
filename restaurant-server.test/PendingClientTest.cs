using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Text;
using communication_lib;
using System.Threading;
using System.Threading.Tasks;
using Moq;

namespace restaurant_server.test
{
    class PendingClientTest
    {
        Mock<IConnectionHandler> _connectionHandler;
        Mock<IModel> _model;
        Mock<IClient> _IClient;
        PendingClient _client;
        CancellationTokenSource _tokenSource = new CancellationTokenSource();

        [SetUp]
        public void SetUp()
        {
            _connectionHandler = new Mock<IConnectionHandler>();
            _model = new Mock<IModel>();
            _IClient = new Mock<IClient>();
            _client = new PendingClient(_IClient.Object, _model.Object, _connectionHandler.Object);
        }

        [TearDown]
        public void TearDown()
        {
            _IClient.VerifyNoOtherCalls();
            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequestedAdmin_Successful()
        {
            _model
                .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>()))
                .Returns((string user, string pass) =>
                    {
                        if (user == "Admin" && pass == "adminpassword")
                        {
                            return Task.FromResult(LoginResult.Admin);
                        }
                        return Task.FromResult(LoginResult.Deny);
                    });

            await _client.LoginRequested("Admin", "adminpassword", _tokenSource.Token);

            _IClient
                .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, _tokenSource.Token));
            _connectionHandler
                .Verify(ch => ch.CreateAdmin("Admin", _IClient.Object));
        }

        [Test]
        public async Task LoginRequestedAdmin_WrongPassword()
        {
            _model
               .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>()))
               .Returns((string user, string pass) =>
               {
                   if (user == "Admin" && pass == "adminpassword")
                   {
                       return Task.FromResult(LoginResult.Admin);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });

            await _client.LoginRequested("Admin", "cica", _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequestedAdmin_EmptyPassword()
        {
            _model
               .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>()))
               .Returns((string user, string pass) =>
               {
                   if (user == "Admin" && pass == "adminpassword")
                   {
                       return Task.FromResult(LoginResult.Admin);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });

            await _client.LoginRequested("Admin", "", _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequestedCustomer_Successful()
        {
            _model
               .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>()))
               .Returns((string user, string pass) =>
               {
                   if (user == "Table" && pass == "table")
                   {
                       return Task.FromResult(LoginResult.Customer);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });

            await _client.LoginRequested("Table", "table", _tokenSource.Token);

            _IClient
                .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, _tokenSource.Token));
            _connectionHandler
                .Verify(ch => ch.CreateCustomer("Table", _IClient.Object));
        }

        [Test]
        public async Task LoginRequested_WrongUsername()
        {
            _model
              .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>()))
              .Returns((string user, string pass) =>
              {
                  if (user == "Table" && pass == "tablepassword")
                  {
                      return Task.FromResult(LoginResult.Customer);
                  }
                  return Task.FromResult(LoginResult.Deny);
              });

            await _client.LoginRequested("Table_", "tablepassword", _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequesteCustomer_EmptyPassword()
        {
            _model
             .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>()))
             .Returns((string user, string pass) =>
             {
                 if (user == "Table" && pass == "tablepassword")
                 {
                     return Task.FromResult(LoginResult.Customer);
                 }
                 return Task.FromResult(LoginResult.Deny);
             });

            await _client.LoginRequested("Table", "", _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }
    }
}
