using communication_lib;
using Moq;
using NUnit.Framework;
using NUnit.Framework.Constraints;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server.test
{
    public class CustomerClientTest
    {
        Mock<IConnectionHandler> _connectionHandler;
        Mock<IModel> _model;
        Mock<IClient> _IClient;
        CustomerClient _client;
        string name = "Table";


        CancellationTokenSource _tokenSource = new CancellationTokenSource();
        [SetUp]
        public void Setup()
        {
            _connectionHandler = new Mock<IConnectionHandler>();
            _model = new Mock<IModel>();
            _IClient = new Mock<IClient>();
            _client = new CustomerClient(_IClient.Object, name, _model.Object, _connectionHandler.Object);
        }

        [TearDown]
        public void TearDown()
        {
            _IClient.VerifyNoOtherCalls();
            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task FoodListRequest_AllVisible()
        {
            // Arrange
            var expectedFoods = new List<Food>
            {
                new Food
                {
                    Visible = true,
                    FoodData = new FoodContains
                    {
                        FoodId = 0,
                        FoodName = "Gulyás",
                        Amount = 10,
                        FoodPrice = 500
                    }
                },
                new Food
                {
                    Visible = true,
                    FoodData = new FoodContains
                    {
                        FoodId = 1,
                        FoodName = "Palacsinta",
                        Amount = 5,
                        FoodPrice = 250
                    }
                }
            };
            _model
                .Setup(m => m.ListFoods(It.IsAny<bool>())).Returns((bool Visible) =>
                {
                    return Task.FromResult<IEnumerable<Food>>(expectedFoods);
                });

            // Act
            await _client.HandleMessage(new FoodListRequestMessage { }, _tokenSource.Token);

            // Assert
            _IClient
                .Verify(c => c.Send(It.Is<FoodListReplyMessage>(msg =>
                    msg.Foods.SequenceEqual(expectedFoods.Select(f => f.FoodData))
                ), _tokenSource.Token));
        }

        [Test]
        public async Task OrderRequest_Successful()
        {
            // Arrange
            var expectedFoods = new List<FoodContains>
            {
                new FoodContains
                {
                    FoodId = 0,
                    FoodName = "Palacsinta",
                    Amount = 5,
                    FoodPrice = 250
                }
            };
            var expectedOrder = new Orders
            {
                OrderId = 1,
                OrderedFoods = expectedFoods,
                OrderDate = (UInt64)DateTime.Now.Ticks,
                TableId = "Table",
                Status = OrderStatus.Pending
            };
            _model
               .Setup(m => m.AddOrder(It.IsAny<string>(), It.IsAny<List<FoodAmount>>())).Returns((string name, List<FoodAmount> orderedfood) =>
              {
                  var result = new OrderResult
                  {
                      Success = true,
                      Order = expectedOrder
                  };
                  return Task.FromResult(result);
              });

            // Act
            await _client.HandleMessage(new OrderRequestMessage(), _tokenSource.Token);

            // Assert
            _IClient
               .Verify(c => c.Send(It.Is<OrderReplyMessage>(msg =>
                    msg.Orderedfoods.SequenceEqual(expectedFoods)
                ), _tokenSource.Token));

            _connectionHandler
                .Verify(ch => ch.BroadcastToAdmins(It.Is<NotificationOrdersMessage>(msg =>
                    TestHelper.OrdersAreEqual(expectedOrder).Invoke(msg.Order)
                )));
        }

        [Test]
        public async Task OrderRequest_Failed()
        {
            // Arrange
            var expectedFoods = new List<FoodContains>
            {
                new FoodContains
                {
                    FoodId = 0,
                    FoodName = "Palacsinta",
                    Amount = 5,
                    FoodPrice = 250
                }
            };
            var expectedOrder = new Orders
            {
                OrderId = 1,
                OrderedFoods = expectedFoods,
                OrderDate = (UInt64)DateTime.Now.Ticks,
                TableId = "Table",
                Status = OrderStatus.Pending
            };
            _model
               .Setup(m => m.AddOrder(It.IsAny<string>(), It.IsAny<List<FoodAmount>>())).Returns(((string name, List<FoodAmount> orderedfood) =>
               {
                   var result = new OrderResult
                   {
                       Success = false,
                       Order = expectedOrder
                   };
                   return Task.FromResult(result);
               }));

            // Act
            await _client.HandleMessage(new OrderRequestMessage(), _tokenSource.Token);

            // Assert
            _IClient
               .Verify(c => c.Send(It.Is<OrderReplyMessage>(msg =>
                    msg.Orderedfoods.Count == 0), _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task PayRequest_Successful()
        {
            // Arrange
            var expectedFoods = new List<FoodContains>
            {
                new FoodContains
                {
                    FoodId = 0,
                    FoodName = "Palacsinta",
                    Amount = 5,
                    FoodPrice = 250
                }
            };

            var expectedOrder = new Orders
            {
                OrderId = 1,
                OrderedFoods = expectedFoods,
                OrderDate = (UInt64)DateTime.Now.Ticks,
                TableId = "Table",
                Status = OrderStatus.Completed

            };

            _model.
                Setup(m => m.TryPay(It.IsAny<string>())).Returns((string success) =>
                  {
                      var result = new PayResult
                      {
                          Success = true,
                          Order = expectedOrder
                      };
                      return Task.FromResult(result);
                  });

            // Act
            await _client.HandleMessage(new PayRequestMessage(), _tokenSource.Token);
            
            // Assert
            _IClient
               .Verify(c => c.Send(It.Is<PayReplyMessage>(msg =>
                    msg.Status ==PayStatus.Success), _tokenSource.Token));

            _connectionHandler
                .Verify(ch => ch.BroadcastToAdmins(It.Is<NotificationOrdersMessage>(msg =>
                    TestHelper.OrdersAreEqual(expectedOrder).Invoke(msg.Order)
                )));
        }

        [Test]
        public async Task PayRequest_Failed()
        {
            // Arrange
            var expectedFoods = new List<FoodContains>
            {
                new FoodContains
                {
                    FoodId = 0,
                    FoodName = "Palacsinta",
                    Amount = 5,
                    FoodPrice = 250
                }
            };

            var expectedOrder = new Orders
            {
                OrderId = 1,
                OrderedFoods = expectedFoods,
                OrderDate = (UInt64)DateTime.Now.Ticks,
                TableId = "Table",
                Status = OrderStatus.Completed

            };

            _model.
                Setup(m => m.TryPay(It.IsAny<string>())).Returns((string success) =>
                {
                    var result = new PayResult
                    {
                        Success = false,
                        Order = expectedOrder
                    };
                    return Task.FromResult(result);
                });

            // Act
            await _client.HandleMessage(new PayRequestMessage(), _tokenSource.Token);

            // Assert
            _IClient
               .Verify(c => c.Send(It.Is<PayReplyMessage>(msg =>
                    msg.Status == PayStatus.Failed), _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        public static IEnumerable<Message> unhandledMessages = new List<Message>
        {

            new LoginRequestMessage { },
            new LoginReplyMessage { },
            new FoodListReplyMessage { },
            new OrderReplyMessage { },
            new PayReplyMessage { },
            new OrderArrivedReplyMessage { },
            new OrderArrivedRequestMessage { },
            new NotificationOrdersMessage{ },
            new CompleteFoodRequestMessage { },
            new CompleteFoodReplyMessage { },
            new FoodChangeRequestMessage { },
            new FoodChangeReplyMessage { },
            new OrderStatusChangeRequestMessage { },
            new OrderStatusChangeReplyMessage { },
        };
        [Test]
        public async Task UnhandledMessages_AreNotHandled([ValueSource(nameof(unhandledMessages))] Message msg)
        {
            await _client.HandleMessage(msg, _tokenSource.Token);

            _model.VerifyNoOtherCalls();
            _IClient.VerifyNoOtherCalls();
            _connectionHandler.VerifyNoOtherCalls();
        }

    }
}