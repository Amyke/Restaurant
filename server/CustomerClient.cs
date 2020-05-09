using Castle.Core.Internal;
using communication_lib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{
    internal class CustomerClient
    {
        private readonly IConnectionHandler _connectionHandler;
        private readonly IModel _model;
        public IClient IClient { get; internal set; }

        public string Name { get; internal set; }

        public CustomerClient(IClient client, string name, IModel model, IConnectionHandler connectionHandler)
        {
            _connectionHandler = connectionHandler;
            _model = model;
            IClient = client;
            Name = name;
        }

        public async Task HandleMessage(Message request, CancellationToken cancellation)
        {
            switch (request.Id)
            {
                case MessageId.FoodListRequest:
                    await HandleFoodListRequest((FoodListRequestMessage)request, cancellation);
                    break;
                case MessageId.OrderRequest:
                    await HandleOrderRequestArrived((OrderRequestMessage)request, cancellation);
                    break;
                case MessageId.PayRequest:
                    await HandlePayRequestArrived((PayRequestMessage)request, cancellation);
                    break;
                case MessageId.LoginRequest:
                case MessageId.LoginReply:
                case MessageId.FoodListReply:
                case MessageId.OrderReply:
                case MessageId.PayReply:
                case MessageId.OrderArrivedRequest:
                case MessageId.OrderArrivedReply:
                case MessageId.NotificationOrders:
                case MessageId.CompleteFoodRequest:
                case MessageId.CompleteFoodReply:
                case MessageId.FoodChangeRequest:
                case MessageId.FoodChangeReply:
                case MessageId.OrderStatusChangeRequest:
                case MessageId.OrderStatusChangeReply:
                    // error
                    break;
                default:
                    // error
                    break;
            }
        }

        private async Task HandleFoodListRequest(FoodListRequestMessage msg, CancellationToken cancellation)
        {
            var foods = (await _model.ListFoods(true)).Select(f => f.FoodData).OrderBy(x => x.FoodName).ToList();
            await IClient.Send(new FoodListReplyMessage { Foods = foods }, cancellation);
        }

        private async Task HandleOrderRequestArrived(OrderRequestMessage msg, CancellationToken cancellation)
        {
            if (msg.Orderedfood.Count == 0)
            {
                await IClient.Send(new OrderReplyMessage
                {
                    Orderedfoods = new List<FoodContains>()
                }, cancellation);
                return;
            }
            var result = await _model.AddOrder(Name, msg.Orderedfood);
            var foodsToAdmins = (await _model.ListFoods(false)).OrderBy(x => x.FoodData.FoodName).ToList();
            var foodsToCustomers = (await _model.ListFoods(true)).Select(x => x.FoodData).OrderBy(x => x.FoodName).ToList();
            if (result.Success)
            {
                await IClient.Send(new OrderReplyMessage
                {
                    OrderId = result.Order!.OrderId,
                    Orderedfoods = result.Order!.OrderedFoods
                }, cancellation);
                await _connectionHandler.BroadcastToAdmins(new NotificationOrdersMessage
                {
                    Order = result.Order!
                });
                await _connectionHandler.BroadcastToAdmins(new CompleteFoodReplyMessage
                {
                    FoodData = foodsToAdmins
                });
                await _connectionHandler.BrodcastToCustomers(new FoodListReplyMessage
                {
                    Foods = foodsToCustomers
                });
            }
            else
            {
                await IClient.Send(new OrderReplyMessage
                {
                    Orderedfoods = new List<FoodContains>()
                }, cancellation);
            }
        }

        private async Task HandlePayRequestArrived(PayRequestMessage msg, CancellationToken cancellation)
        {
            PayResult result = await _model.TryPay(msg.OrderId, Name);
            if (result.Success)
            {
                await _connectionHandler.BroadcastToAdmins(new NotificationOrdersMessage
                {
                    Order = result.Order!
                });
            }
            else
            {
                await IClient.Send(new PayReplyMessage { Status = PayStatus.Failed }, cancellation);
            }
        }
    }
}
