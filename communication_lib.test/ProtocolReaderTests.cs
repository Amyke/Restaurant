using communication_lib.Implementation;
using MessagePack;
using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using FluentAssertions;

namespace communication_lib.test
{
    class ProtocolReaderTests
    {
        Stream _stream;
        Client _client;
        ProtocolReader _reader;
        List<Message> _messages;

        [SetUp]
        public void SetUp()
        {
            _stream = new MemoryStream();
            _client = new Client(null,"address");
            _reader = new ProtocolReader(_stream, _client);
            _messages = new List<Message>();

            _client.MessageArrived += (sender, message) =>
            {
                _messages.Add(message);
            };
        }

        [Test]
        public async Task LoginRequestMessage()
        {
            var expected = new LoginRequestMessage
            {
                Name = "user name",
                Password = "**-**"
            };

            await SendMessage(expected);

            _stream.Seek(0, SeekOrigin.Begin);

            var tokenSource = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSource.Token);

            Assert.AreEqual(1, _messages.Count);
            var msgLogin = _messages[0];
            Assert.IsInstanceOf<LoginRequestMessage>(msgLogin);
            var loginReq = (LoginRequestMessage)msgLogin;
            Assert.AreEqual("user name", loginReq.Name);
            Assert.AreEqual("**-**", loginReq.Password);

            _messages[0].Should().BeEquivalentTo(expected);
        }
        
        class Valami
        {
            public List<string> data { get; set; }
        }

        [Test]
        public async Task OrderRequestMessages()
        {
            // Arrange
            var expected = new OrderRequestMessage
            {
                Orderedfood = new List<FoodAmount>()
                {
                    new FoodAmount
                    {
                        FoodId = 10,
                        Amount = 1
                    },
                    new FoodAmount
                    {
                        FoodId = 1,
                        Amount = 10
                    }
                }
            };
            await SendMessage(expected);
            _stream.Seek(0, SeekOrigin.Begin);

            // Act
            var tokenSourceOrder = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSourceOrder.Token);

            // Assert
            Assert.AreEqual(1, _messages.Count);

            _messages[0].Should().BeEquivalentTo(expected);
            //Assert.AreEqual(expected, actual);
        }

        [Test]
        public async Task PayReguestMessage()
        {
            var expected = new PayRequestMessage { PayIntent = true };
            await SendMessage(expected);
            _stream.Seek(0, SeekOrigin.Begin);

            var tokenSourcePay = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSourcePay.Token);

            Assert.AreEqual(1, _messages.Count);
            Assert.AreEqual(expected, _messages[0]);
        }

        [Test]
        public async Task LoginRequestReply()
        {
            var expected = new LoginReplyMessage { Status = LoginStatus.Ok };

            await SendMessage(expected);

            _stream.Seek(0, SeekOrigin.Begin);

            var tokenSourceLogin = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSourceLogin.Token);

            Assert.AreEqual(1, _messages.Count);
            Assert.AreEqual(expected, _messages[0]);
        }

        [Test]
        public async Task OrderRequestReply()
        {
            var expected = new OrderRequestMessage
            {
                Orderedfood = new List<FoodAmount>()
                {
                    new FoodAmount
                    {
                        FoodId = 22,
                        Amount = 1
                    }
                }
            };
            await SendMessage(expected);

            _stream.Seek(0, SeekOrigin.Begin);
            var tokenSourceOrder = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSourceOrder.Token);

            Assert.AreEqual(1, _messages.Count);
            _messages[0].Should().BeEquivalentTo(expected);
        }

        [Test]
        public async Task PayRequestReply()
        {
            var expected = new PayReplyMessage
            {
                Status = PayStatus.Failed
            };

            await SendMessage(expected);

            _stream.Seek(0, SeekOrigin.Begin);
            var tokenSourcePay = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSourcePay.Token);

            Assert.AreEqual(1, _messages.Count);
            var msg = _messages[0];
            Assert.IsInstanceOf<PayReplyMessage>(msg);
            var payRep = (PayReplyMessage)msg;

            Assert.AreEqual(expected, _messages[0]);
        }

        [Test]
        public async Task MessageSequence()
        {
            var expected1 = new PayReplyMessage { Status = PayStatus.Failed };
            var expected2 = new LoginReplyMessage { Status = LoginStatus.Error };
            var expected3 = new PayReplyMessage { Status = PayStatus.Success };

            await SendMessage(expected1);
            await SendMessage(expected2);
            await SendMessage(expected3);

            _stream.Seek(0, SeekOrigin.Begin);
            var tokenSource = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSource.Token);

            Assert.AreEqual(3, _messages.Count);
            Assert.IsInstanceOf<PayReplyMessage>(_messages[0]);
            Assert.IsInstanceOf<LoginReplyMessage>(_messages[1]);
            Assert.IsInstanceOf<PayReplyMessage>(_messages[2]);

            Assert.AreEqual(expected1, _messages[0]);
            Assert.AreEqual(expected2,_messages[1]);
            Assert.AreEqual(expected3, _messages[2]);
        }

        [Test]
        public async Task InterleavedSendReceive()
        {
            var expected1 = new PayReplyMessage { Status = PayStatus.Failed };
            var expected2 = new LoginReplyMessage { Status = LoginStatus.Error };
            var expected3 = new PayReplyMessage { Status = PayStatus.Success };

            await SendMessage(expected1);
            await SendMessage(expected2);

            _stream.Seek(0, SeekOrigin.Begin);
            var tokenSource = new CancellationTokenSource();
            await _reader.ProcessMessages(tokenSource.Token);

            var posAfterFirstReceive = _stream.Position;

            await SendMessage(expected3);

            _stream.Seek(posAfterFirstReceive, SeekOrigin.Begin);
            await _reader.ProcessMessages(tokenSource.Token);

            Assert.AreEqual(3, _messages.Count);
            Assert.IsInstanceOf<PayReplyMessage>(_messages[0]);
            Assert.IsInstanceOf<LoginReplyMessage>(_messages[1]);
            Assert.IsInstanceOf<PayReplyMessage>(_messages[2]);

            Assert.AreEqual(expected1, _messages[0]);
            Assert.AreEqual(expected2, _messages[1]);
            Assert.AreEqual(expected3, _messages[2]);
        }

       private async Task SendMessage(Message msg)
       {
            await MessagePackSerializer.SerializeAsync(_stream, new MessageHeader { Id = msg.Id });
            await MessagePackSerializer.SerializeAsync(msg.GetType(), _stream, msg);
            await _stream.FlushAsync();
        }
    }
}
