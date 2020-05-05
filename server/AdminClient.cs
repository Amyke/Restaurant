using communication_lib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{
    class AdminClient
    {
        private readonly ConnectionHandler _connectionHandler;
        private readonly IModel _model;
        public IClient IClient { get; internal set; }
        public string Name { get; internal set; }

        public AdminClient(IClient client, string name, IModel model, ConnectionHandler connectionHandler)
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
            var foods = (await _model.ListFoods(false)).ToList();
            await IClient.Send(new CompleteFoodReplyMessage { FoodData = foods }, cancellation);
        }
        private async Task HandleOrderArrivedRequest(OrderArrivedRequestMessage msg, CancellationToken cancellation)
        {
            //model->data from database from-to List<Orders>

            var orders = (await _model.ListOrders(
                    new DateTime((long)msg.FromDate),
                    new DateTime((long)msg.ToDate)
                )).ToList();
            await IClient.Send(new OrderArrivedReplyMessage { Orders = orders }, cancellation);
        }
        private async Task HandleFoodChangeRequest(FoodChangeRequestMessage msg, CancellationToken cancellation)
        {
            //model-> success delta change (bool)
            bool success = true;
            if (success)
            {
                await IClient.Send(new FoodChangeReplyMessage { Status = FoodChangeStatus.Success }, cancellation);
            }
            else
            {
                await IClient.Send(new FoodChangeReplyMessage { Status = FoodChangeStatus.Failed }, cancellation);
            }
        }
        private async Task HandleOrderStatusChangeRequest(OrderStatusChangeRequestMessage msg, CancellationToken cancellation)
        {
            UInt64 date = Convert.ToUInt64(DateTime.Now);

            //model->update status
            if (handleStatusChange(msg.Status) == msg.Status)
            {
                await IClient.Send(new OrderStatusChangeReplyMessage { OrderId = msg.OrderId, Status = ReplyStatus.Success, NewStatus = msg.Status, Date = date }, cancellation);
            }
            else
            {
                await IClient.Send(new OrderStatusChangeReplyMessage { OrderId = msg.OrderId, Status = ReplyStatus.Success, NewStatus = msg.Status, Date = date }, cancellation);
            }
        }
        private OrderStatus handleStatusChange(OrderStatus status)
        {
            // TODO: switch-case
            if (status == OrderStatus.Pending)
            {
                return OrderStatus.InProgress;
            }
            else if (status == OrderStatus.InProgress)
            {
                return OrderStatus.Completed;
            }
            else if (status == OrderStatus.Completed)
            {
                return OrderStatus.Payed;
            }
            throw new NotImplementedException();
        }
    }
}
