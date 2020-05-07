using communication_lib;
using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace restaurant_server
{
    public enum LoginResult
    {
        Customer,
        Admin,
        Deny
    }

    public struct PayResult
    {
        public bool Success { get; set; }
        public Orders? Order { get; set; }
    }

    public struct OrderResult
    {
        public bool Success { get; set; }
        public Orders? Order { get; set; }
    }

    public struct OrderStatusChangeResult
    {
        public bool Success { get; set; }
        public UInt64? OrderId { get; set; }
        public OrderStatus? NewStatus { get; set; }
        public UInt64? Date { get; set; }
    }

    public interface IModel
    {
        Task<LoginResult> Login(string name, string password);
        Task<IEnumerable<Food>> ListFoods(bool visibleOnly);
        Task<OrderResult> AddOrder(string name, List<FoodAmount> orderedfood);
        Task<PayResult> TryPay(UInt64 orderId, string tableId);
        Task<IEnumerable<Orders>> ListOrders(DateTimeOffset from, DateTimeOffset to);

        Task<bool> FoodChange(Delta changes);

        Task<OrderStatusChangeResult> StatusChange(UInt64 orderId, OrderStatus status);
    }
}
