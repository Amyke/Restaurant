using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class OrderStatusChangeRequestMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.OrderStatusChangeRequest;

        [Key(0)]
        public UInt64 OrderId { get; set; }
        [Key(1)]
        public OrderStatus Status { get; set; }
    }
}
