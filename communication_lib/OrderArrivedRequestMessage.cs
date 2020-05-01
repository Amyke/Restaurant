using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public struct OrderArrivedRequestMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.OrderArrivedRequest;

        [Key(0)]
        public UInt64 FromDate { get; set; }
        [Key(1)]
        public UInt64 ToDate { get; set; }
    }
}
