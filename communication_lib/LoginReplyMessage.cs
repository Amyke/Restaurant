
using MessagePack;
using System;
using System.Collections.Generic;
using System.Text;

namespace communication_lib
{
    public enum LoginStatus
    {
        Ok = 0,
        Error
    }

    [MessagePackObject]
    public struct LoginReplyMessage : Message
    {
        [IgnoreMember]
        public MessageId Id => MessageId.LoginReply;

        [Key(0)]
        public LoginStatus Status { get; set; }
    }
}