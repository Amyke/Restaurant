using Castle.DynamicProxy.Generators;
using communication_lib;
using Microsoft.VisualBasic;
using Moq;
using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server.test
{
    class AdminClientTest
    {

        Mock<IConnectionHandler> _connectionHandler;
        Mock<IModel> _model;
        Mock<IClient> _IClient;
        AdminClient _client;
        string name = "Admin";


        CancellationTokenSource _tokenSource = new CancellationTokenSource();

        [SetUp]
        public void SetUp()
        {
            _connectionHandler = new Mock<IConnectionHandler>();
            _model = new Mock<IModel>();
            _IClient = new Mock<IClient>();
            _client = new AdminClient(_IClient.Object, name, _model.Object, _connectionHandler.Object);
        }

        [TearDown]
        public void TearDown()
        {
            _IClient.VerifyNoOtherCalls();
            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task CompleteFoodRequest()
        {
            // Arrange
            var expectedFoods = new List<Food>
            {
                new Food
                {
                    Visible = false,
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
            await _client.HandleMessage(new CompleteFoodRequestMessage { }, _tokenSource.Token);

            // Assert
            _IClient
                .Verify(c => c.Send(It.Is<CompleteFoodReplyMessage>(msg =>
                    TestHelper.FoodsAreEqual(expectedFoods).Invoke(msg.FoodData))
                , _tokenSource.Token));
        }

        [Test]
        public async Task OrderArrivedRequest()
        {
            // Arrange
            var expectedOrder = new List<Orders>
            {
                new Orders
                {
                OrderId = 1,
                OrderedFoods = new List<FoodContains>
                {
                    new FoodContains
                    {
                        FoodId = 0,
                        FoodName = "Bundáskenyér",
                        Amount = 5,
                        FoodPrice = 250
                    }
                },
                OrderDate = (UInt64)DateTimeOffset.UtcNow.ToUnixTimeSeconds(),
                TableId = "Table",
                Status = OrderStatus.Pending
                }
            };

            _model
                .Setup(m => m.ListOrders(It.IsAny<DateTimeOffset>(), It.IsAny<DateTimeOffset>()))
                .Returns((DateTimeOffset from, DateTimeOffset to) =>
               {
                   return Task.FromResult<IEnumerable<Orders>>(expectedOrder);
               });

            // Act
            await _client.HandleMessage(new OrderArrivedRequestMessage { }, _tokenSource.Token);

            // Assert
            _IClient
                .Verify(c => c.Send(It.Is<OrderArrivedReplyMessage>(msg =>
                    msg.Orders.SequenceEqual(expectedOrder))
                    , _tokenSource.Token));
        }

        [Test]
        public async Task FoodChangeRequest_Successful()
        {
            // Arrange
            _model.
                Setup(m => m.FoodChange(It.IsAny<Delta>())).Returns((Delta delta) =>
               {
                   return Task.FromResult(true);
               });

            // Act
            await _client.HandleMessage(new FoodChangeRequestMessage { }, _tokenSource.Token);

            // Assert
            _connectionHandler.
                Verify(ch => ch.BroadcastToAdmins(It.Is<FoodChangeReplyMessage>(msg =>
                    msg.Status == FoodChangeStatus.Success)
                ));

            _IClient.VerifyNoOtherCalls();
        }

        [Test]
        public async Task FoodChangeRequest_Failed()
        {
            // Arrange
            _model.
                Setup(m => m.FoodChange(It.IsAny<Delta>())).Returns((Delta delta) =>
                {
                    return Task.FromResult(false);
                });

            // Act
            await _client.HandleMessage(new FoodChangeRequestMessage { }, _tokenSource.Token);

            // Assert
            _IClient
                .Verify(c => c.Send(It.Is<FoodChangeReplyMessage>(msg =>
                msg.Status == FoodChangeStatus.Failed)
                , _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        [Test]
        public async Task OrderStatusChangeRequest_Successful()
        {
            // Arrange
            var expectedChange = new OrderStatusChangeResult
            {
                OrderId = 0,
                Date = (UInt64)DateTime.Now.Ticks,
                NewStatus = OrderStatus.InProgress,
                Success = true
            };
            _model
                .Setup(m => m.StatusChange(It.IsAny<UInt64>(), It.IsAny<OrderStatus>())).Returns((UInt64 orderId, OrderStatus status) =>
                {
                    return Task.FromResult(expectedChange);
                });

            // Act
            await _client.HandleMessage(new OrderStatusChangeRequestMessage { }, _tokenSource.Token);

            // Assert
            var expected = new OrderStatusChangeReplyMessage
            {
                OrderId = expectedChange.OrderId.Value,
                Date = expectedChange.Date.Value,
                NewStatus = expectedChange.NewStatus.Value,
                Status = ReplyStatus.Success
            };

            _connectionHandler
                .Verify(ch => ch.BroadcastToAdmins(It.Is<OrderStatusChangeReplyMessage>(msg =>
                    TestHelper.OrderChangeAreEqual(expected).Invoke(msg)
                )));

            _IClient.VerifyNoOtherCalls();
        }

        [Test]
        public async Task OrderStatusChangeRequest_Failed()
        {
            // Arrange
            var expectedChange = new OrderStatusChangeResult
            {
                OrderId = 42,
                Date = (UInt64)DateTimeOffset.UtcNow.ToUnixTimeSeconds(),
                NewStatus = OrderStatus.InProgress,
                Success = false
            };
            _model
                .Setup(m => m.StatusChange(It.IsAny<UInt64>(), It.IsAny<OrderStatus>()))
                .Returns((UInt64 orderId, OrderStatus status) =>
                {
                    return Task.FromResult(expectedChange);
                });

            // Act
            await _client.HandleMessage(new OrderStatusChangeRequestMessage { OrderId = 42, Status = OrderStatus.Payed }, _tokenSource.Token);

            // Assert
            _IClient
                .Verify(c => c.Send(It.Is<OrderStatusChangeReplyMessage>(msg =>
                    msg.OrderId == expectedChange.OrderId
                    && msg.NewStatus == OrderStatus.Payed
                    && msg.Status == ReplyStatus.Failed)
                , _tokenSource.Token));

            _connectionHandler.VerifyNoOtherCalls();
        }

        public static IEnumerable<Message> unhandledMessages = new List<Message>
        {
            new LoginRequestMessage{ },
            new LoginReplyMessage{ },
            new FoodListRequestMessage{ },
            new FoodListReplyMessage{ },
            new OrderRequestMessage{ },
            new OrderReplyMessage{ },
            new PayRequestMessage{ },
            new PayReplyMessage{ },
            new OrderArrivedReplyMessage{ },
            new NotificationOrdersMessage{ },
            new CompleteFoodReplyMessage{ },
            new FoodChangeReplyMessage{ },
            new OrderStatusChangeReplyMessage{ }

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
