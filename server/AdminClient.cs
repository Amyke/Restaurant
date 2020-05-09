using communication_lib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{
    internal class AdminClient
    {
        private readonly IConnectionHandler _connectionHandler;
        private readonly IModel _model;
        public IClient IClient { get; internal set; }
        public string Name { get; internal set; }

        public AdminClient(IClient client, string name, IModel model, IConnectionHandler connectionHandler)
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
                case MessageId.CompleteFoodRequest:
                    {
                        var msg = (CompleteFoodRequestMessage)request;
                        await HandleCompleteFoodRequest(msg, cancellation);
                        break;
                    }
                case MessageId.OrderArrivedRequest:
                    {
                        var msg = (OrderArrivedRequestMessage)request;
                        await HandleOrderArrivedRequest(msg, cancellation);
                        break;
                    }
                case MessageId.FoodChangeRequest:
                    {
                        var msg = (FoodChangeRequestMessage)request;
                        await HandleFoodChangeRequest(msg, cancellation);
                        break;
                    }
                case MessageId.OrderStatusChangeRequest:
                    {
                        var msg = (OrderStatusChangeRequestMessage)request;
                        await HandleOrderStatusChangeRequest(msg, cancellation);
                        break;
                    }
                case MessageId.LoginRequest:
                case MessageId.LoginReply:
                case MessageId.FoodListRequest:
                case MessageId.FoodListReply:
                case MessageId.OrderRequest:
                case MessageId.OrderReply:
                case MessageId.PayRequest:
                case MessageId.PayReply:
                case MessageId.OrderArrivedReply:
                case MessageId.NotificationOrders:
                case MessageId.CompleteFoodReply:
                case MessageId.FoodChangeReply:
                case MessageId.OrderStatusChangeReply:
                    // error
                    break;
            }
        }


        private async Task HandleCompleteFoodRequest(CompleteFoodRequestMessage msg, CancellationToken cancellation)
        {
            var foods = (await _model.ListFoods(false)).OrderBy(x=>x.FoodData.FoodName).ToList();
            await IClient.Send(new CompleteFoodReplyMessage { FoodData = foods }, cancellation);
        }
        private async Task HandleOrderArrivedRequest(OrderArrivedRequestMessage msg, CancellationToken cancellation)
        {
            //model->data from database from-to List<Orders>
            var orders = (await _model.ListOrders(
                    DateTimeOffset.FromUnixTimeSeconds((long)msg.FromDate),
                    DateTimeOffset.FromUnixTimeSeconds((long)msg.ToDate)
                )).ToList();

            await IClient.Send(new OrderArrivedReplyMessage { Orders = orders }, cancellation);
        }
        private async Task HandleFoodChangeRequest(FoodChangeRequestMessage msg, CancellationToken cancellation)
        {
            //model-> success delta change (bool)
            bool success = await _model.FoodChange(msg.Changes);
            if (success)
            {
                await _connectionHandler.BroadcastToAdmins(new FoodChangeReplyMessage
                {
                    Status = FoodChangeStatus.Success
                });
            }
            else
            {
                await IClient.Send(new FoodChangeReplyMessage { Status = FoodChangeStatus.Failed }, cancellation);
            }
        }
        private async Task HandleOrderStatusChangeRequest(OrderStatusChangeRequestMessage msg, CancellationToken cancellation)
        {
            //model->update status
            OrderStatusChangeResult result = await _model.StatusChange(msg.OrderId, msg.Status);


            if (result.Success)
            {
                await _connectionHandler.BroadcastToAdmins(new OrderStatusChangeReplyMessage
                {
#pragma warning disable CS8629 // Nullable value type may be null.
                    OrderId = result.Order!.OrderId,
                    Status = ReplyStatus.Success,
                    NewStatus = result.Order!.Status,
                    Date = result.Order!.OrderDate
#pragma warning restore CS8629 // Nullable value type may be null.
                });

                if (result.Order.Status == OrderStatus.Payed)
                {
                    await _connectionHandler.SendToCustomer(result.Order!.TableId, new PayReplyMessage { Status = PayStatus.Success });
                }
                else
                {

                    await _connectionHandler.SendToCustomer(result.Order!.TableId, new NotificationOrdersMessage
                    {
                        Order = result.Order!
                    });
                }
            }
            else
            {
                await IClient.Send(new OrderStatusChangeReplyMessage
                {
                    OrderId = msg.OrderId,
                    Status = ReplyStatus.Failed,
                    NewStatus = msg.Status
                }, cancellation);

            }

        }
    }
}
