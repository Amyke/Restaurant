using communication_lib.Implementation;
using MessagePack;
using System.Buffers;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace communication_lib
{
    internal class ProtocolReader
    {
        Stream _stream;
        Client _client;

        public ProtocolReader(Stream stream, Client client)
        {
            _stream = stream;
            _client = client;
        }

        public async Task ProcessMessages(CancellationToken cancellation)
        {
            var reader = new MessagePackStreamReader(_stream);
            while (await reader.ReadAsync(cancellation) is ReadOnlySequence<byte> headerBuffer)
            {
                var header = MessagePackSerializer.Deserialize<MessageHeader>(headerBuffer, cancellationToken: cancellation);
                if (await reader.ReadAsync(cancellation) is ReadOnlySequence<byte> bodyBuffer)
                {
                    var message = DecodeMessage(header.Id, bodyBuffer, cancellation);
                    if (message != null)
                    {
                        _client.OnMessageArrived(message);
                    }
                }
            }
        }

        private static Message DecodeMessage(MessageId id, ReadOnlySequence<byte> buffer, CancellationToken cancellationToken)
        {
            return id switch
            {
                MessageId.LoginRequest => MessagePackSerializer.Deserialize<LoginRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.LoginReply => MessagePackSerializer.Deserialize<LoginReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.FoodListRequest => MessagePackSerializer.Deserialize<FoodListRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.FoodListReply => MessagePackSerializer.Deserialize<FoodListReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.OrderRequest => MessagePackSerializer.Deserialize<OrderRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.OrderReply => MessagePackSerializer.Deserialize<OrderReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.PayRequest => MessagePackSerializer.Deserialize<PayRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.PayReply => MessagePackSerializer.Deserialize<PayReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.FoodChangeRequest => MessagePackSerializer.Deserialize<FoodChangeRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.FoodChangeReply => MessagePackSerializer.Deserialize<FoodChangeReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.OrderArrivedRequest => MessagePackSerializer.Deserialize<OrderArrivedRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.OrderArrivedReply => MessagePackSerializer.Deserialize<OrderArrivedReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.NotificationOrders => MessagePackSerializer.Deserialize<NotificationOrdersMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.CompleteFoodRequest => MessagePackSerializer.Deserialize<CompleteFoodRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.CompleteFoodReply => MessagePackSerializer.Deserialize< CompleteFoodReplyMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.OrderStatusChangeRequest => MessagePackSerializer.Deserialize<OrderStatusChangeRequestMessage>(buffer, cancellationToken: cancellationToken),
                MessageId.OrderStatusChangeReply => MessagePackSerializer.Deserialize<OrderStatusChangeReplyMessage>(buffer, cancellationToken: cancellationToken),
                _ => throw new System.NotImplementedException()
            };
        }
    }
}
