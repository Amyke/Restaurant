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
                .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
                .Returns((string user, string pass, bool admin) =>
                    {
                        if (user == "Admin" && pass == "adminpassword" && admin == true)
                        {
                            return Task.FromResult(LoginResult.Admin);
                        }
                        return Task.FromResult(LoginResult.Deny);
                    });

            await _client.LoginRequested("Admin", "adminpassword", true, _tokenSource.Token);

            _IClient
                .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, _tokenSource.Token));
            _connectionHandler
                .Verify(ch => ch.CreateAdmin("Admin", _IClient.Object));
        }

        [Test]
        public async Task LoginRequestedAdmin_WrongPassword()
        {
            _model
               .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
               .Returns((string user, string pass, bool admin) =>
               {
                   if (user == "Admin" && pass == "adminpassword" && admin==true)
                   {
                       return Task.FromResult(LoginResult.Admin);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });

            await _client.LoginRequested("Admin", "cica", true, _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequestedAdmin_EmptyPassword()
        {
            _model
               .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
               .Returns((string user, string pass, bool admin) =>
               {
                   if (user == "Admin" && pass == "adminpassword" && admin==true)
                   {
                       return Task.FromResult(LoginResult.Admin);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });

            await _client.LoginRequested("Admin", "",true,  _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequestedCustomer_Successful()
        {
            _model
               .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
               .Returns((string user, string pass, bool admin) =>
               {
                   if (user == "Table" && pass == "table" && admin==false)
                   {
                       return Task.FromResult(LoginResult.Customer);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });
            _connectionHandler
                .Setup(c => c.GetLoggedInCustomers())
                .Returns(new List<string> { });

            await _client.LoginRequested("Table", "table",false, _tokenSource.Token);

            _IClient
                .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, _tokenSource.Token));
            _connectionHandler
                .Verify(ch => ch.CreateCustomer("Table", _IClient.Object));
            _connectionHandler.Verify(ch => ch.GetLoggedInCustomers());
        }

        [Test]
        public async Task LoginRequestCustomers_Successful()
        {
            _model.
                Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
               .Returns((string user, string pass, bool admin) =>
               {
                   if (user == "Table" && pass == "table" && admin==false|| user == "test" && pass == "test" && admin==false)
                   {
                       return Task.FromResult(LoginResult.Customer);
                   }
                   return Task.FromResult(LoginResult.Deny);
               });
            _connectionHandler
                .Setup(c => c.GetLoggedInCustomers())
                .Returns(new List<string> { "test" });

            await _client.LoginRequested("Table", "table",false, _tokenSource.Token);

            _IClient
                .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Ok }, _tokenSource.Token));
            _connectionHandler
                .Verify(ch => ch.CreateCustomer("Table", _IClient.Object));
            _connectionHandler.Verify(ch => ch.GetLoggedInCustomers());
        }

        [Test]
        public async Task LoginRequestedCustomers_LoggedInAlready()
        {
            _model.
               Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>() ))
              .Returns((string user, string pass, bool admin) =>
              {
                  if (user == "Table" && pass == "table" && admin==false || user == "test" && pass == "test" && admin==false)
                  {
                      return Task.FromResult(LoginResult.Customer);
                  }
                  return Task.FromResult(LoginResult.Deny);
              });
            _connectionHandler
                .Setup(c => c.GetLoggedInCustomers())
                .Returns(new List<string> { "Table" });

            await _client.LoginRequested("Table", "table",false, _tokenSource.Token);

            _IClient
                .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));
            _connectionHandler.Verify(ch => ch.GetLoggedInCustomers());
        }

        [Test]
        public async Task LoginRequested_WrongUsername()
        {
            _model
              .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
              .Returns((string user, string pass, bool admin) =>
              {
                  if (user == "Table" && pass == "tablepassword" && admin==false)
                  {
                      return Task.FromResult(LoginResult.Customer);
                  }
                  return Task.FromResult(LoginResult.Deny);
              });

            await _client.LoginRequested("Table_", "tablepassword",false, _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task LoginRequesteCustomer_EmptyPassword()
        {
            _model
             .Setup(m => m.Login(It.IsAny<string>(), It.IsAny<string>(), It.IsAny<bool>()))
             .Returns((string user, string pass, bool admin) =>
             {
                 if (user == "Table" && pass == "tablepassword" && admin==false)
                 {
                     return Task.FromResult(LoginResult.Customer);
                 }
                 return Task.FromResult(LoginResult.Deny);
             });

            await _client.LoginRequested("Table", "",false,  _tokenSource.Token);

            _IClient
               .Verify(c => c.Send(new LoginReplyMessage { Status = LoginStatus.Error }, _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }
    }
}
