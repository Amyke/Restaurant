using communication_lib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace restaurant_server.test
{
    class TestHelper
    {
        public static Func<Orders, bool> OrdersAreEqual(Orders expected)
        {
            return actual =>
                expected.OrderId == actual.OrderId &&
                expected.Status == actual.Status &&
                expected.TableId == actual.TableId &&
                expected.OrderDate == actual.OrderDate &&
                actual.OrderedFoods.SequenceEqual(expected.OrderedFoods);
        }

        public static Func<List<Food>, bool> FoodsAreEqual(List<Food> expected)
        {
            return actual =>
            expected.SequenceEqual(actual);

        }

        public static Func<List<FoodContains>, bool> FoodContainsAreEqual(List<FoodContains> expected)
        {
            return actual =>
            expected.SequenceEqual(actual);

        }
        public static Func<OrderStatusChangeReplyMessage, bool> OrderChangeAreEqual(OrderStatusChangeReplyMessage expected)
        {
            return actual =>
            expected.OrderId == actual.OrderId &&
            expected.Status == actual.Status &&
            expected.NewStatus == actual.NewStatus &&
            expected.Date == actual.Date;
        }
    }
}
