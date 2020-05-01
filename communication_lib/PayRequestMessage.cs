using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public struct PayRequestMessage : Message
    {
        [IgnoreMember]
        MessageId Message.Id => MessageId.PayRequest;

        [Key(0)]
        public bool PayIntent { get; set; }
    }
}
