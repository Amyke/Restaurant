using communication_lib;
using MessagePack;
using System;
using System.Collections.Generic;

namespace communication_lib
{
    public enum MessageId : UInt32
    {
        LoginRequest = 0,
        LoginReply,
        FoodListRequest,
        FoodListReply,
        OrderRequest,
        OrderReply,
        PayRequest,
        PayReply,
        OrderArrivedRequest,
        OrderArrivedReply,
        NotificationOrders,
        CompleteFoodRequest,
        CompleteFoodReply,
        FoodChangeRequest,
        FoodChangeReply,
        OrderStatusChangeRequest,
        OrderStatusChangeReply
    }

    public enum OrderStatus : UInt32
    {
        Pending = 0,
        InProgress,
        Completed,
        Payed,
        PayIntent
    }

    [MessagePackObject]
    public struct FoodContains
    {
        [Key(0)]
        public UInt32 FoodId { get; set; }
        [Key(1)]
        public string FoodName { get; set; }
        [Key(2)]
        public UInt32 FoodPrice { get; set; }
        [Key(3)]
        public UInt32 Amount { get; set; }
    }

    [MessagePackObject]
    public struct FoodAmount
    {
        [Key(0)]
        public UInt32 FoodId { get; set; }
        [Key(1)]
        public UInt32 Amount { get; set; }
    };

    [MessagePackObject]
    public class Orders
    {
        [Key(0)]
        public UInt64 OrderId { get; set; }
        [Key(1)]
        public string TableId { get; set; }
        [Key(2)]
        public List<FoodContains> OrderedFoods { get; set; } = new List<FoodContains>();
        [Key(3)]
        public UInt64 OrderDate { get; set; }
        [Key(4)]
        public OrderStatus Status { get; set; }
    };

    [MessagePackObject]
    public class Food
    {
        [Key(0)]
        public FoodContains FoodData { get; set; }
        [Key(1)]
        public bool Visible { get; set; }
    };

    [MessagePackObject]
    public class Delta
    {
        [Key(0)]
        public List<Food> ModifiedFoods { get; set; } = new List<Food>();
        [Key(1)]
        public List<Food> CreatedFoods { get; set; } = new List<Food>();
    };

    public interface Message
    {
        MessageId Id { get; }
    }
}