using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class OrderArrivedReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.OrderArrivedReply;

        [Key(0)]
        public List<Orders> Orders { get; set; } = new List<Orders>();
    }
}
