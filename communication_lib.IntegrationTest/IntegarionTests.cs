using communication_lib.Implementation;
using FluentAssertions;
using MessagePack;
using NUnit.Framework;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;
using static communication_lib.IntegrationTest.Extensions;

namespace communication_lib.IntegrationTest
{
    [TestFixture]
    public class IntegarionTests
    {
        private CancellationTokenSource _cancellationSource = new CancellationTokenSource();
        private Communication _communication = new Communication(new IPEndPoint(IPAddress.Any, 0));
        private List<IClient> _connectedClients = new List<IClient>();
        

        [OneTimeSetUp]
        public void SetupAsync()
        {
            _communication.Listen(_cancellationSource.Token).ConfigureAwait(false);
            _communication.ConnectionEstablished += (s, client) =>
            {
                _connectedClients.Add(client);
                client.ConnectionLost += (s, _) =>
                {
                    _connectedClients.Remove(client);
                };
            };
        }

        [Test, Order(0)]
        public async Task ConnectAndClose()
        {
            var client = await Connect();

            Assert.AreEqual(1, await WaitForClients(1));

            await client.Close();
            await Task.Delay(100);
            Assert.AreEqual(0, _connectedClients.Count);
        }

        [Test, Order(1)]
        public async Task SendMessage()
        {
            await using var client = await Connect();

            Assert.AreEqual(1, await WaitForClients(1));

            var msg = new LoginRequestMessage { Name = "☠", Password = "valami" };
            var messages = _connectedClients[0].Messages();
            await client.Send(msg, _cancellationSource.Token);

            Assert.AreEqual(1, _connectedClients.Count);
            var actual = await messages.Receive();
            actual.Should().BeEquivalentTo(msg);
        }

        [Test, Order(2)]
        public async Task SendMultipleMessage()
        {
            await using var client = await Connect();

            Assert.AreEqual(1, await WaitForClients(1));

            var msg1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var msg2 = new LoginRequestMessage { Name = "☠", Password = "valami" };
            var msg3 = new LoginReplyMessage { Status = LoginStatus.Ok };

            var messages = _connectedClients[0].Messages();
            await client.Send(msg1, _cancellationSource.Token);
            await client.Send(msg2, _cancellationSource.Token);
            await client.Send(msg3, _cancellationSource.Token);

            (await messages.Receive()).Should().BeEquivalentTo(msg1);
            (await messages.Receive()).Should().BeEquivalentTo(msg2);
            (await messages.Receive()).Should().BeEquivalentTo(msg3);
        }

        [Test, Order(3)]
        public async Task SingleMessageFromMultipleClient()
        {
            await using var client1 = await Connect();
            await using var client2 = await Connect();
            await using var client3 = await Connect();

            Assert.AreEqual(3, await WaitForClients(3));

            var msg1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var msg2 = new LoginRequestMessage { Name = "☠", Password = "valami" };
            var msg3 = new LoginReplyMessage { Status = LoginStatus.Ok };


            var messages1 = _connectedClients[0].Messages();
            var messages2 = _connectedClients[1].Messages();
            var messages3 = _connectedClients[2].Messages();
            await client1.Send(msg1, _cancellationSource.Token);
            await client2.Send(msg2, _cancellationSource.Token);
            await client3.Send(msg3, _cancellationSource.Token);

            Assert.AreEqual(3, _connectedClients.Count);


            (await messages1.Receive()).Should().BeEquivalentTo(msg1);
            (await messages2.Receive()).Should().BeEquivalentTo(msg2);
            (await messages3.Receive()).Should().BeEquivalentTo(msg3);
        }

        [Test, Order(4)]
        public async Task MultipleMessageFromMultipleClient()
        {
            await using var client1 = await Connect();
            await using var client2 = await Connect();
            await using var client3 = await Connect();

            Assert.AreEqual(3, await WaitForClients(3));

            var msg1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var msg2 = new LoginRequestMessage { Name = "☠", Password = "valami" };
            var msg3 = new LoginReplyMessage { Status = LoginStatus.Ok };

            var messages1 = _connectedClients[0].Messages();
            var messages2 = _connectedClients[1].Messages();
            var messages3 = _connectedClients[2].Messages();

            await client1.Send(msg1, _cancellationSource.Token);
            await client1.Send(msg2, _cancellationSource.Token);
            await client1.Send(msg3, _cancellationSource.Token);

            await client2.Send(msg2, _cancellationSource.Token);
            await client2.Send(msg3, _cancellationSource.Token);
            await client2.Send(msg1, _cancellationSource.Token);

            await client3.Send(msg3, _cancellationSource.Token);
            await client3.Send(msg1, _cancellationSource.Token);
            await client3.Send(msg2, _cancellationSource.Token);

            (await messages1.Receive()).Should().BeEquivalentTo(msg1);
            (await messages1.Receive()).Should().BeEquivalentTo(msg2);
            (await messages1.Receive()).Should().BeEquivalentTo(msg3);

            (await messages2.Receive()).Should().BeEquivalentTo(msg2);
            (await messages2.Receive()).Should().BeEquivalentTo(msg3);
            (await messages2.Receive()).Should().BeEquivalentTo(msg1);

            (await messages3.Receive()).Should().BeEquivalentTo(msg3);
            (await messages3.Receive()).Should().BeEquivalentTo(msg1);
            (await messages3.Receive()).Should().BeEquivalentTo(msg2);
        }

        [Test, Order(5)]
        public async Task SeveralClientsSendMessagesAtOnce()
        {
            var msg1 = new LoginReplyMessage { Status = LoginStatus.Error };
            List<Task<IClient>> clientTasks = new List<Task<IClient>>();
            for (int i = 0; i < 400; ++i)
            {
                clientTasks.Add(Task.Run(() => Connect()));
            }

            var clients = new List<IClient>(await Task.WhenAll(clientTasks));
            Assert.AreEqual(clients.Count, await WaitForClients(clients.Count, TimeSpan.FromSeconds(2)));

            var messages = _connectedClients
                .Select(c => c.Messages())
                .ToList();

            await Task.WhenAll(clients.Select(
                client => client.Send(msg1, _cancellationSource.Token)
            ));

            foreach (var queue in messages)
            {
                (await queue.Receive(TimeSpan.FromSeconds(5))).Should().BeEquivalentTo(msg1);
            }

            foreach (IClient client in clients)
            {
                await client.Close();
            }
        }

        [Test, Order(6)]
        public async Task SendMessageToClient()
        {
            await using var client = await Connect();
            var msg = new LoginReplyMessage { Status = LoginStatus.Error };

            var messages = client.Messages();
            await _connectedClients[0].Send(msg, _cancellationSource.Token);

            (await messages.Receive()).Should().BeEquivalentTo(msg);

        }

        [Test, Order(7)]
        public async Task MultipleMessageToClient()
        {
            await using var client = await Connect();
            var msg1 = new OrderRequestMessage
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
            var msg2 = new LoginReplyMessage { Status = LoginStatus.Error };

            var message = client.Messages();
            await _connectedClients[0].Send(msg1, _cancellationSource.Token);

            await _connectedClients[0].Send(msg2, _cancellationSource.Token);

            (await message.Receive()).Should().BeEquivalentTo(msg1);
            (await message.Receive()).Should().BeEquivalentTo(msg2);

        }
        
        [Test, Order(8)]
        public async Task MessageToMultipleClient()
        {
            await using var client1 = await Connect();
            await using var client2 = await Connect();
            await using var client3 = await Connect();


            Assert.AreEqual(3, await WaitForClients(3));

            var msg1 = new LoginReplyMessage { Status = LoginStatus.Error };
            var msg2 = new LoginReplyMessage { Status = LoginStatus.Ok };
            var msg3 = new PayReplyMessage { Status = PayStatus.Failed };

            var message1 = client1.Messages();
            var message2 = client2.Messages();
            var message3 = client3.Messages();

            await _connectedClients[0].Send(msg1, _cancellationSource.Token);
            await _connectedClients[1].Send(msg2, _cancellationSource.Token);
            await _connectedClients[2].Send(msg3, _cancellationSource.Token);

            (await message1.Receive()).Should().BeEquivalentTo(msg1);
            (await message2.Receive()).Should().BeEquivalentTo(msg2);
            (await message3.Receive()).Should().BeEquivalentTo(msg3);
        }

        [Test, Order(9)]
        public async Task SendAndReciveMessage()
        {
            // Arrange
            await using var client = await Connect();
            Assert.AreEqual(1, await WaitForClients(1));

            var msg1 = new LoginRequestMessage { Name = "🦊🦊🦊", Password = "***-***" };
            var msg2 = new LoginReplyMessage { Status = LoginStatus.Error };

            var clientToServer = _connectedClients[0].Messages();
            var serverToClient = client.Messages();

            // Act
            await client.Send(msg1, _cancellationSource.Token);
            await _connectedClients[0].Send(msg2, _cancellationSource.Token);

            // Assert
            (await clientToServer.Receive()).Should().BeEquivalentTo(msg1);
            (await serverToClient.Receive()).Should().BeEquivalentTo(msg2);
        }

        [Test, Order(10)]
        public async Task SpeakMultipleClients()
        {
            await using var client1 = await Connect();
            await using var client2 = await Connect();
            await using var client3 = await Connect();

            Assert.AreEqual(3, await WaitForClients(3));

            var msg11 = new LoginRequestMessage { Name = "🦊🦊🦊", Password = "***-***" };
            var msg12 = new LoginReplyMessage { Status = LoginStatus.Error };
            var msg21 = new LoginRequestMessage { Name = "ELTEIK", Password = "Szamhalok" };
            var msg22 = new LoginReplyMessage { Status = LoginStatus.Ok };
            var msg31 = new PayRequestMessage { PayIntent = true };
            var msg32 = new PayReplyMessage { Status = PayStatus.Failed };

            var messagetoserver1 = _connectedClients[0].Messages();
            await client1.Send(msg11, _cancellationSource.Token);

            var replaytoclient1 = client1.Messages();
            await _connectedClients[0].Send(msg12, _cancellationSource.Token);

            var messagetoserver2 = _connectedClients[1].Messages();
            var messagetoserver3 = _connectedClients[2].Messages();
            await client2.Send(msg21, _cancellationSource.Token);
            await client3.Send(msg31, _cancellationSource.Token);

            var replaytoclient2 = client2.Messages();
            await _connectedClients[1].Send(msg22, _cancellationSource.Token);
            var replaytoclient3 = client3.Messages();
            await _connectedClients[2].Send(msg32, _cancellationSource.Token);

            (await messagetoserver1.Receive()).Should().BeEquivalentTo(msg11);
            (await replaytoclient1.Receive()).Should().BeEquivalentTo(msg12);

            (await messagetoserver2.Receive()).Should().BeEquivalentTo(msg21);
            (await messagetoserver3.Receive()).Should().BeEquivalentTo(msg31);
            (await replaytoclient2.Receive()).Should().BeEquivalentTo(msg22);
            (await replaytoclient3.Receive()).Should().BeEquivalentTo(msg32);
        }

        [Test, Order(11)]
        public async Task SendMessageToSeveralClientsAtOnce()
        {
            var msg1 = new LoginReplyMessage { Status = LoginStatus.Error };
            List<Task<IClient>> clientTasks = new List<Task<IClient>>();
            for (int i = 0; i < 400; ++i)
            {
                clientTasks.Add(Task.Run(() => Connect()));
            }

            var clients = new List<IClient>(await Task.WhenAll(clientTasks));
            Assert.AreEqual(clients.Count, await WaitForClients(clients.Count, TimeSpan.FromSeconds(2)));


            var messages = clients
                .Select(c => c.Messages())
                .ToList();

            await Task.WhenAll(_connectedClients.Select(
            server => server.Send(msg1, _cancellationSource.Token)
            ));

            foreach (var queue in messages)
            {
                (await queue.Receive(TimeSpan.FromSeconds(5))).Should().BeEquivalentTo(msg1);
            }

            foreach (IClient client in clients)
            {
                await client.Close();
            }
        }
        [Test, Order(12)]
        public async Task EchoOneClient()
        {
            await using var client = await Connect();

            Assert.AreEqual(1, await WaitForClients(1));

            var messagetoserver = new LoginRequestMessage { Name = "☠", Password = "valami" };
            var messages = _connectedClients[0].Messages();
            await client.Send(messagetoserver, _cancellationSource.Token);

            var msg = await messages.Receive();
            msg.Should().BeEquivalentTo(messagetoserver);

            var ansver =  client.Messages();
            await _connectedClients[0].Send(msg, _cancellationSource.Token);

            (await ansver.Receive()).Should().BeEquivalentTo(messagetoserver);
        }

        [Test, Order(13)]
        public async Task EchoMultiplateClient()
        {
            await using var client1 = await Connect();
            await using var client2 = await Connect();
            await using var client3 = await Connect();


            Assert.AreEqual(3, await WaitForClients(3));

            var msg1 = new LoginRequestMessage { Name = "🦊🦊🦊", Password = "***-***" };
            var msg2 = new LoginRequestMessage { Name = "ELTEIK", Password = "Szamhalok" };
            var msg3 = new LoginRequestMessage { Name = "Nummod1", Password = "Szeretjuk" };

            var messages1 = _connectedClients[0].Messages();
            var messages2 = _connectedClients[1].Messages();
            var messages3 = _connectedClients[2].Messages();

            await client1.Send(msg1, _cancellationSource.Token);
            await client2.Send(msg2, _cancellationSource.Token);
            await client3.Send(msg3, _cancellationSource.Token);

            var msg1_receive = await messages1.Receive();
            var msg2_receive = await messages2.Receive();
            var msg3_receive = await messages3.Receive();

            msg1_receive.Should().BeEquivalentTo(msg1);
            msg2_receive.Should().BeEquivalentTo(msg2);
            msg3_receive.Should().BeEquivalentTo(msg3);

            var ansver1 = client1.Messages();
            var ansver2 = client2.Messages();
            var ansver3 = client3.Messages();
            await _connectedClients[0].Send(msg1_receive, _cancellationSource.Token);
            await _connectedClients[1].Send(msg2_receive, _cancellationSource.Token);
            await _connectedClients[2].Send(msg3_receive, _cancellationSource.Token);

            (await ansver1.Receive()).Should().BeEquivalentTo(msg1);
            (await ansver2.Receive()).Should().BeEquivalentTo(msg2);
            (await ansver3.Receive()).Should().BeEquivalentTo(msg3);

        }

        private async Task<IClient> Connect()
        {
            return await _communication.Connect(new IPEndPoint(IPAddress.Parse("127.0.0.1"), _communication.ListenAddress.Port), _cancellationSource.Token);
        }

        public async Task<int> WaitForClients(int count, TimeSpan? timeout = null)
        {
            var deadline = DateTime.Now + (timeout ?? TimeSpan.FromMilliseconds(100));
            while (DateTime.Now <= deadline)
            {
                if (_connectedClients.Count == count)
                {
                    return count;
                }
                await Task.Delay(10);
            }
            return _connectedClients.Count;
        }
    }



    static class Extensions
    {
        public class MessageQueue
        {
            BufferBlock<Message> _queue = new BufferBlock<Message>();

            public MessageQueue(IClient client)
            {
                client.MessageArrived += async (s, msg) =>
                {
                    await _queue.SendAsync(msg);
                };
            }

            public async Task<Message> Receive(TimeSpan? timeout = null)
            {
                return await _queue.ReceiveAsync(timeout ?? TimeSpan.FromMilliseconds(500));
            }
        }

        public static MessageQueue Messages(this IClient client)
        {
            return new MessageQueue(client);
        }
    }
}