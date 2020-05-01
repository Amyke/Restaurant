using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public struct FoodListReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.FoodListReply;

        [Key(0)]
        public List<FoodContains> Foods { get; set; }
    }
    
}
