using communication_lib;
using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Data;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace restaurant_server
{
    class Model : IModel
    {
        Func<Persistence.RestaurantContext> _contextFactory;

        public Model(Func<Persistence.RestaurantContext> databaseContextFactory)
        {
            _contextFactory = databaseContextFactory;
        }

        async Task<LoginResult> IModel.Login(string name, string password)
        {
            using var data = CreateDatabase();
            var pass = SHA512.Create().ComputeHash(Encoding.UTF8.GetBytes(password));
            var user = await data.Users.FirstOrDefaultAsync(u => u.Name == name);
            if (user != null && user.Password.SequenceEqual(pass))
            {
                return user.IsAdmin
                    ? LoginResult.Admin
                    : LoginResult.Customer;
            }
            return LoginResult.Deny;
        }

        async Task<IEnumerable<Food>> IModel.ListFoods(bool visibleOnly)
        {
            using var data = CreateDatabase();
            return await data.FoodAmounts
                .Where(f => !visibleOnly || f.Food.Visible)
                .Select(f => new Food
                {
                    FoodData = new FoodContains
                    {
                        FoodId = (UInt32)f.FoodId,
                        Amount = (UInt32)f.Amount,
                        FoodName = f.Food.Name,
                        FoodPrice = (UInt32)f.Food.Price
                    },
                    Visible = f.Food.Visible
                })
                .ToListAsync();
        }

        async Task<OrderResult> IModel.AddOrder(string name, List<FoodAmount> orderedfood)
        {
            using var data = CreateDatabase();
            using var trx = await data.Database.BeginTransactionAsync();
            var user = await data.Users.FirstOrDefaultAsync(u => u.Name == name);
            if (user == null)
            {
                // error
                return new OrderResult { Success = false };
            }

            var dbFoods = new Dictionary<Persistence.FoodAmount, int>();
            foreach (var ordered in orderedfood)
            {
                var db = await data.FoodAmounts.FindAsync((int)ordered.FoodId);
                if (db.Amount < ordered.Amount)
                {
                    // error
                    return new OrderResult { Success = false };
                }
                dbFoods.Add(db, (int)ordered.Amount);
            }

            var order = new Persistence.Order
            {
                Status = Persistence.DbOrderStatus.Pending,
                Date = DateTime.UtcNow,
                Table = user
            };
            order = (await data.AddAsync(order)).Entity;

            var orderFoodAmounts = dbFoods.Select(pair => new Persistence.OrderFoodAmount
            {
                Order = order,
                Food = pair.Key.Food,
                Amount = pair.Value
            });
            await data.AddRangeAsync(orderFoodAmounts);
            await data.SaveChangesAsync();
            await trx.CommitAsync();

            return new OrderResult
            {
                Success = true,
                Order = new Orders
                {
                    TableId = name,
                    OrderDate = (UInt64)order.Date.ToUnixTimeSeconds(),
                    OrderedFoods = orderFoodAmounts.Select(ofa => new FoodContains
                    {
                        FoodId = (UInt32)ofa.Food.Id,
                        FoodName = ofa.Food.Name,
                        Amount = (UInt32)ofa.Amount,
                        FoodPrice = (UInt32)ofa.Food.Price
                    }).ToList(),
                    OrderId = (UInt32)order.Id,
                    Status = OrderStatus.Pending
                }
            };
        }

        async Task<PayResult> IModel.TryPay(UInt64 orderId, string tableId)
        {
            using var data = CreateDatabase();
            //model engedélyezi-e -->success
            //model payrequestből csinál egy payrequestet amibe benne van az orderId
            using var trx = await data.Database.BeginTransactionAsync();

            var status = await data.Orders.FindAsync((int)orderId);
            if (status.Status != Persistence.DbOrderStatus.Completed || status.Table.Name != tableId)
            {
                return new PayResult { Success = false };
            }
            status.Status = Persistence.DbOrderStatus.PayIntent;

            await data.SaveChangesAsync();
            await trx.CommitAsync();

            List<FoodContains> foods = status.Foods.Select(x => new FoodContains
            {
                FoodId = (UInt32)x.Food.Id,
                FoodName = x.Food.Name,
                Amount = (UInt32)x.Amount,
                FoodPrice = (UInt32)x.Food.Price
            }).ToList();


            return new PayResult
            {
                Success = true,
                Order = new Orders
                {
                    TableId = status.Table.Name,
                    OrderDate = (UInt64)DateTimeOffset.UtcNow.ToUnixTimeSeconds(),
                    OrderedFoods = foods,
                    OrderId = (UInt32)status.Id,
                    Status = OrderStatus.PayIntent
                }
            };


        }

        async Task<IEnumerable<Orders>> IModel.ListOrders(DateTimeOffset from, DateTimeOffset to)
        {
            if (DateTimeOffset.Compare(from, to) > 0)
            {
                DateTimeOffset tmp = from;
                from = to;
                to = tmp;
            }

            using var data = CreateDatabase();
            return await data.Orders
                .Where(o => from <= o.Date && o.Date <= to)
                .Select(item => new Orders
                {
                    OrderId = (UInt32)item.Id,
                    OrderDate = (UInt64)item.Date.ToLocalTime().ToUnixTimeSeconds(),
                    Status = fromDb(item.Status),
                    TableId = item.Table.Name,
                    OrderedFoods = item.Foods.Select(x => new FoodContains
                    {
                        Amount = (UInt32)x.Amount,
                        FoodId = (UInt32)x.FoodId,
                        FoodName = x.Food.Name,
                        FoodPrice = (UInt32)x.Food.Price
                    }).ToList()
                }).ToListAsync();
        }
        async Task<bool> IModel.FoodChange(Delta changes)
        {
            using var data = CreateDatabase();
            using var trx = await data.Database.BeginTransactionAsync();

            var modifiedFoods = new List<Persistence.FoodAmount>();
            foreach (var food in changes.ModifiedFoods)
            {
                var dbFood = await data.FoodAmounts.FindAsync((int)food.FoodData.FoodId);

                if (dbFood == null)
                {
                    return false;
                }

                dbFood.Amount = (int)food.FoodData.Amount;
                dbFood.Food.Name = food.FoodData.FoodName;
                dbFood.Food.Price = (int)food.FoodData.FoodPrice;
                dbFood.Food.Visible = food.Visible;
            }

            var createdFoods = changes.CreatedFoods.Select(created => new Persistence.FoodAmount
            {
                Amount = (int)created.FoodData.Amount,
                Food = new Persistence.Food
                {
                    Name = created.FoodData.FoodName,
                    Price = (int)created.FoodData.FoodPrice,
                    Visible = created.Visible
                }
            });

            await data.AddRangeAsync(createdFoods);
            await data.SaveChangesAsync();
            await trx.CommitAsync();

            return true;
        }

        async Task<OrderStatusChangeResult> IModel.StatusChange(UInt64 orderId, OrderStatus status)
        {
            using var data = CreateDatabase();
            using var trx = await data.Database.BeginTransactionAsync();
            var order = data.Orders.Find((int)orderId);
            if (order == null)
            {
                return new OrderStatusChangeResult { Success = false };
            }

            switch (order.Status, status)
            {
                case (Persistence.DbOrderStatus.Pending, OrderStatus.InProgress):
                    order.Status = Persistence.DbOrderStatus.InProgress;
                    break;
                case (Persistence.DbOrderStatus.InProgress, OrderStatus.Completed):
                    order.Status = Persistence.DbOrderStatus.Completed;
                    break;
                case (Persistence.DbOrderStatus.Completed, OrderStatus.PayIntent):
                    order.Status = Persistence.DbOrderStatus.Payed;
                    break;
                case (Persistence.DbOrderStatus.PayIntent, OrderStatus.Payed):
                    order.Status = Persistence.DbOrderStatus.Payed;
                    break;
                default:
                    // error
                    return new OrderStatusChangeResult { Success = false };
            }

            order.Date = DateTime.UtcNow;
            await data.SaveChangesAsync();
            await trx.CommitAsync();
            return new OrderStatusChangeResult
            {
                Success = true,
                Order = new Orders
                {
                    OrderId = (UInt64)order.Id,
                    OrderDate = (UInt64)order.Date.ToUnixTimeSeconds(),
                    Status = fromDb(order.Status),
                    TableId = order.Table.Name,
                    OrderedFoods = order.Foods.Select(f => new FoodContains
                    {
                        FoodId = (UInt32)f.FoodId,
                        Amount = (UInt32)f.Amount,
                        FoodName = f.Food.Name,
                        FoodPrice = (UInt32)f.Food.Price
                    }).ToList()
                }
            };
        }

        private Persistence.RestaurantContext CreateDatabase() => _contextFactory.Invoke();

        private static OrderStatus fromDb(Persistence.DbOrderStatus status)
        {
            return status switch
            {
                Persistence.DbOrderStatus.Pending => OrderStatus.Pending,
                Persistence.DbOrderStatus.InProgress => OrderStatus.InProgress,
                Persistence.DbOrderStatus.Completed => OrderStatus.Completed,
                Persistence.DbOrderStatus.PayIntent => OrderStatus.PayIntent,
                Persistence.DbOrderStatus.Payed => OrderStatus.Payed,
                _ => throw new NotImplementedException()
            };
        }
    }
}
