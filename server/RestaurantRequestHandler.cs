using communication_lib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{

    class RestaurentRequestHandler
    {
        enum MessageState
        {
            SendByCustomer,
            SendToAdmin,
            SendByAdmin,
            SendToCustomer
        }

        public Message ReplyMessage(Message request)
        {
            switch (request.Id)
            {
                case MessageId.LoginRequest:
                    return new LoginReplyMessage { Status = LoginStatus.Error };
                case MessageId.FoodListRequest:
                    return new FoodListReplyMessage { Foods = new List<FoodContains> {} };
                case MessageId.OrderRequest:
                    return new OrderReplyMessage { Orderedfoods = new List<FoodContains>() };
                case MessageId.PayRequest:
                    return new PayReplyMessage { Status = PayStatus.Failed };
                default:
                    throw new NotImplementedException();
            }
        }

        
    }
}