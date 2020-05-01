using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace communication_lib
{
    public interface IClient : IAsyncDisposable
    {
        string Address { get; }

        event EventHandler ConnectionLost;

        event EventHandler<Message> MessageArrived;

        Task Send(Message message, CancellationToken cancellation);
        Task Close();
    }
}
