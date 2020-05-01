using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public sealed class FoodChangeRequestMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.FoodChangeRequest;

        //todo?
        [Key(0)]
        public Delta Changes { get; set; } = new Delta();
    }
}
