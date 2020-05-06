using communication_lib;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace restaurant_server
{
    class Model : IModel
    {
        public Model()
        {
        }

        async Task<LoginResult> IModel.Login(string name, string password)
        {
            //SHA512.Create().ComputeHash(Encoding.UTF8.GetBytes(password));
            if (name == "Table")
            {
                return LoginResult.Customer;
            }
            else if (name == "Admin")
            {
                return LoginResult.Admin;
            }
            else
            {
                return LoginResult.Deny;
            }
        }

        async Task<IEnumerable<Food>> IModel.ListFoods(bool visibleOnly)
        {
            //model.FoodListVisible lekérés és visszaadás
            return new List<Food>
            {
                new Food
                {
                    FoodData = new FoodContains
                    {
                        FoodId = 0,
                        FoodName = "Gulyás",
                        Amount = 10,
                        FoodPrice = 500
                    },
                    Visible = visibleOnly
                }
            };
        }

        async Task<OrderResult> IModel.AddOrder(string name, List<FoodAmount> orderedfood)
        {
            //model.OrderId hozzáadás
            //model FoodAmount->FoodContains csinálás
            //model elmentés
            //model  FoodContainsből Orders csinálás -> /* string OrderId, string Name, List<FoodContains>, UInt64 OrderDate OrderStatus Status */

            return new OrderResult
            {
                Success = true,
                Order = new Orders
                {
                    TableId = name,
                    OrderDate = (UInt64)DateTime.UtcNow.Ticks,
                    OrderedFoods = orderedfood.Select(f => new FoodContains
                    {
                        FoodId = f.FoodId,
                        FoodName = "asd",
                        Amount = f.Amount,
                        FoodPrice = 1000
                    }).ToList(),
                    OrderId = 0,
                    Status = OrderStatus.Pending
                }
            };
        }

        async Task<PayResult> IModel.TryPay(string tableId)
        {
            //model engedélyezi-e -->success
            //model payrequestből csinál egy payrequestet amibe benne van az orderId

            return new PayResult
            {
                Success = false,
                Order = new Orders
                {
                    TableId = tableId,
                    OrderDate = (UInt64)DateTime.UtcNow.Ticks,
                    OrderedFoods = new List<FoodContains>(),
                    OrderId = 0,
                    Status = OrderStatus.Completed
                }
            };
        }

        async Task<IEnumerable<Orders>> IModel.ListOrders(DateTime from, DateTime to)
        {
            if (DateTime.Compare(from, to) > 0)
            {
                DateTime tmp = from;
                from = to;
                to = tmp;
            }

            return new List<Orders>
            {
            };
        }
        async Task<bool> IModel.FoodChange(Delta changes)
        {
           bool success=true;
            return success;
        }

        async Task<OrderStatusChangeResult> IModel.StatusChange(UInt64 orderId, OrderStatus status)
        {
            bool success = true;
            if(success)
            {
                return new OrderStatusChangeResult
                {
                    OrderId = orderId,
                    Date = (UInt64)DateTime.Now.Ticks,
                    NewStatus = status,
                    Success = success
                };
            }
            else
            {
                return new OrderStatusChangeResult {
                    OrderId = orderId,
                    Date = (UInt64)DateTime.Now.Ticks,
                    NewStatus = status,
                    Success = success
                };
            }
        }

        private OrderStatus handleStatusChange(OrderStatus status)
        {
            switch (status)
            {
                case OrderStatus.Pending:
                    return OrderStatus.InProgress;
                case OrderStatus.InProgress:
                    return OrderStatus.Completed;
                case OrderStatus.Completed:
                    return OrderStatus.Payed;
            }
            throw new NotImplementedException();
        }

    }
}
