
using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    [MessagePackObject]
    public struct LoginRequestMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.LoginRequest;
        [Key(0)]
        public string Name { get; set; }
        [Key(1)]
        public string Password { get; set; }

        [Key(2)]
        public bool IsAdmin { get; set; }

    }
}
