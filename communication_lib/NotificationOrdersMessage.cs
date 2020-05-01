using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class NotificationOrdersMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.NotificationOrders;

        [Key(0)]
        public List<Orders> Orders { get; set; } = new List<Orders>();
    }
}
