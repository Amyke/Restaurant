﻿using communication_lib.Implementation;
using MessagePack;
using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace communication_lib.test
{
    class ProtocolWriterTest
    {
        Stream _stream;
        ProtocolWriter _writer;

        [SetUp]
        public void SetUp()
        {
            _stream = new MemoryStream();
            _writer = new ProtocolWriter(_stream);
        }

        [Test]
        public void EmptyStream()
        {
            var cancellation = new CancellationTokenSource();
            Assert.AreEqual(0, _stream.Position);
        }

        [Test]
        public async Task OneMessage()
        {
            var expected = new LoginReplyMessage
            {
                Status = LoginStatus.Error
            };

            var cancellation = new CancellationTokenSource();

            await _writer.WriteMessage(expected, cancellation.Token);
            _stream.Seek(0, SeekOrigin.Begin);

            var actual = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected.Status, actual.Status);
        }

        [Test]
        public async Task MoreMessageAsync()
        {
            var expected1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var expected2 = new PayReplyMessage { Status = PayStatus.Failed };
            var expected3 = new LoginReplyMessage { Status = LoginStatus.Ok };


            var cancellation = new CancellationTokenSource();

            await _writer.WriteMessage(expected1, cancellation.Token);
            await _writer.WriteMessage(expected2, cancellation.Token);
            await _writer.WriteMessage(expected3, cancellation.Token);

            _stream.Seek(0, SeekOrigin.Begin);

            var actual1 = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected1.Status, actual1.Status);
            var actual2 = await ReceiveMessage<PayReplyMessage>();
            Assert.AreEqual(expected2.Status, actual2.Status);
            var actual3 = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected3.Status, actual3.Status);
        }

        [Test]
        public async Task SendManyMessagesAtOnce()
        {
            var expected1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var expected2 = new PayReplyMessage { Status = PayStatus.Success };
            var expected3 = new OrderReplyMessage { Orderedfoods = new List<FoodContains>(), OrderId = 1 };
            var expected4 = new LoginReplyMessage { Status = LoginStatus.Ok };
            var expected5 = new OrderReplyMessage { Orderedfoods = new List<FoodContains>(), OrderId = 2 };
            var expected6 = new PayReplyMessage { Status = PayStatus.Failed };
            var expected7 = new OrderReplyMessage { Orderedfoods = new List<FoodContains>(), OrderId = 3 };
            var expected = new Message[]
            {
                expected1, expected2, expected3, expected4, expected5, expected6, expected7
            };

            var cancellation = new CancellationTokenSource();
            await Task.WhenAll(expected.Select(msg => _writer.WriteMessage(msg, cancellation.Token)));

            _stream.Seek(0, SeekOrigin.Begin);

            var actual1 = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected1.Status, actual1.Status);
            var actual2 = await ReceiveMessage<PayReplyMessage>();
            Assert.AreEqual(expected2.Status, actual2.Status);
            var actual3 = await ReceiveMessage<OrderReplyMessage>();
            Assert.AreEqual(expected3.OrderId, actual3.OrderId);
            var actual4 = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected4.Status, actual4.Status);
            var actual5 = await ReceiveMessage<OrderReplyMessage>();
            Assert.AreEqual(expected5.OrderId, actual5.OrderId);
            var actual6 = await ReceiveMessage<PayReplyMessage>();
            Assert.AreEqual(expected6.Status, actual6.Status);
            var actual7 = await ReceiveMessage<OrderReplyMessage>();
            Assert.AreEqual(expected7.OrderId, actual7.OrderId);
        }

        [Test]
        public async Task InterleavedMessageAsync()
        {
            var expected1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var expected2 = new PayReplyMessage { Status = PayStatus.Failed };
            var expected3 = new LoginReplyMessage { Status = LoginStatus.Ok };

            var cancellation = new CancellationTokenSource();

            await _writer.WriteMessage(expected1, cancellation.Token);
            var pos1 = _stream.Position;
            _stream.Seek(0, SeekOrigin.Begin);
            var actual1 = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected1.Status, actual1.Status);
            Assert.AreEqual(pos1, _stream.Position);

            await _writer.WriteMessage(expected2, cancellation.Token);
            var pos2 = _stream.Position;
            _stream.Seek(pos1, SeekOrigin.Begin);
            var actual2 = await ReceiveMessage<PayReplyMessage>();
            Assert.AreEqual(expected2.Status, actual2.Status);
            Assert.AreEqual(pos2, _stream.Position);

            await _writer.WriteMessage(expected3, cancellation.Token);
            var pos3 = _stream.Position;
            _stream.Seek(pos2, SeekOrigin.Begin);
            var actual3 = await ReceiveMessage<LoginReplyMessage>();
            Assert.AreEqual(expected3.Status, actual3.Status);
            Assert.AreEqual(pos3, _stream.Position);
        }

        private async Task<T> ReceiveMessage<T>() where T : Message
        {
            var header = await MessagePackSerializer.DeserializeAsync<MessageHeader>(_stream);
            var message = await MessagePackSerializer.DeserializeAsync<T>(_stream);
            Assert.AreEqual(header.Id, message.Id);
            return message;
        }
    }
}
