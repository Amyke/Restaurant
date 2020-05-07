using communication_lib;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{
    public interface IConnectionHandler
    {
        void CreateAdmin(string name, IClient client);
        void CreateCustomer(string name, IClient client);
        Task BroadcastToAdmins(Message msg);
        Task SendToCustomer(string tableId, Message msg);
    }
}
