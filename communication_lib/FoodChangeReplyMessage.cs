using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    public enum FoodChangeStatus
    {
        Success,
        Failed
    }

    [MessagePackObject]
    public sealed class FoodChangeReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.FoodChangeReply;

        [Key(0)]
        public FoodChangeStatus Status { get; set; }
    }
}
