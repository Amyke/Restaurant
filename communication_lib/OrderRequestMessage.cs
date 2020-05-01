using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class OrderRequestMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.OrderRequest;

        [Key(0)]
        public List<FoodAmount> Orderedfood { get; set; } = new List<FoodAmount>();

    }
}
