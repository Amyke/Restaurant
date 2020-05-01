using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class OrderReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.OrderReply;

        [Key(0)]
        public List<FoodContains> Orderedfoods { get; set; } = new List<FoodContains>();
    }
}
