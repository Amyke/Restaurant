using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class FoodListRequestMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.FoodListRequest;

    }
}
