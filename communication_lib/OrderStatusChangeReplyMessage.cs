using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    public enum ReplyStatus
    {
        Success,
        Failed
    }

    [MessagePackObject]
    public sealed class OrderStatusChangeReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.OrderStatusChangeReply;

        [Key(0)]
        public UInt64 OrderId;
        [Key(1)]
        public ReplyStatus Status;
        [Key(2)]
        public OrderStatus NewStatus;
    }
}
