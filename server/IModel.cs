using communication_lib;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace restaurant_server
{
    enum LoginResult
    {
        Customer,
        Admin,
        Deny
    }

    struct PayResult
    {
        public bool Success { get; set; }
        public Orders? Order { get; set; }
    }

    struct OrderResult
    {
        public bool Success { get; set; }
        public Orders? Order { get; set; }
    }

    interface IModel
    {
        Task<LoginResult> Login(string name, string password);
        Task<IEnumerable<Food>> ListFoods(bool visibleOnly);
        Task<OrderResult> AddOrder(string name, List<FoodAmount> orderedfood);
        Task<PayResult> TryPay(string tableId);
        Task<IEnumerable<Orders>> ListOrders(DateTime from, DateTime to);
    }
}
