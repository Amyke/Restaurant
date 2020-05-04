using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class CompleteFoodReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.CompleteFoodReply;

        [Key(0)]
        public List<Food> FoodData { get; set; } = new List<Food>();
    }
}
