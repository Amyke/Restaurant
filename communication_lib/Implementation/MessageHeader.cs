using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib.Implementation
{

    [MessagePackObject]
    public class MessageHeader
    {
        [Key(0)]
        public MessageId Id { get; set; }
    }
}
