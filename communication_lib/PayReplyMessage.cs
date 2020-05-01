using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    public enum PayStatus
    {
        Success,
        Failed
    }

    [MessagePackObject]
    public struct PayReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.PayReply;

        [Key(0)]
        public PayStatus Status { get; set; }
    }
}
