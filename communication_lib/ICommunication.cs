using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace communication_lib
{
    interface ICommunication
    {
        IPEndPoint ListenAddress { get; }
        event EventHandler<IClient> ConnectionEstablished;

        Task Listen(CancellationToken cancellation);
        Task<IClient> Connect(IPEndPoint address, CancellationToken cancellation);
        Task Close();
    }
}
