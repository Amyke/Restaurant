using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Text;

namespace restaurant_server.Persistence
{
    class RestaurantContext : DbContext
    {
        public RestaurantContext() {}

        public RestaurantContext(DbContextOptions<RestaurantContext> options) : base(options)
        {
        }

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            base.OnConfiguring(optionsBuilder);
            optionsBuilder.UseLazyLoadingProxies();
        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);
            modelBuilder.Entity<User>(e =>
            {
                e.HasIndex(c => c.Name).IsUnique();
            });
            modelBuilder.Entity<Food>(e =>
            {
                e.HasIndex(c => c.Name).IsUnique();
            });
            modelBuilder.Entity<FoodAmount>(e =>
            {
                e.HasKey(c => c.FoodId);
            });
            modelBuilder.Entity<OrderFoodAmount>(e =>
            {
                e.HasKey(c => new { c.OrderId, c.FoodId });
            });
        }

        public DbSet<User> Users { get; set; }
        public DbSet<Food> Foods { get; set; }
        public DbSet<FoodAmount> FoodAmounts { get; set; }
        public DbSet<Order> Orders { get; set; }
        public DbSet<OrderFoodAmount> OrderFoodAmounts { get; set; }
    }

    public class User
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public Int32 Id { get; set; }

        [Required]
        public string Name { get; set; }

        [Required]
        public byte[] Password { get; set; }

        [Required]
        public bool IsAdmin { get; set; }
    }

    public class Food
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public Int32 Id { get; set; }

        [Required]
        public string Name { get; set; }

        [Required]
        public int Price { get; set; }

        [Required]
        public bool Visible { get; set; }
    }

    public class FoodAmount
    {
        [Required]
        public Int32 FoodId { get; set; }
        [ForeignKey(nameof(FoodId))]
        virtual public Food Food { get; set; }

        [Required]
        public int Amount { get; set; }
    }

    public enum DbOrderStatus
    {
        Pending = 0,
        InProgress = 1,
        Completed = 2,
        Payed = 3
    }

    public class Order
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public Int32 Id { get; set; }

        [Required]
        virtual public User Table { get; set; }

        [Required]
        public DateTimeOffset Date { get; set; }

        [Required]
        public DbOrderStatus Status { get; set; }

        [Required]
        virtual public IEnumerable<OrderFoodAmount> Foods { get; set; }
    }

    public class OrderFoodAmount
    {
        [Required]
        public Int32 OrderId { get; set; }
        [ForeignKey(nameof(OrderId))]
        virtual public Order Order { get; set; }

        [Required]
        public Int32 FoodId { get; set; }
        [ForeignKey(nameof(FoodId))]
        virtual public Food Food { get; set; }

        [Required]
        public int Amount { get; set; }
    }
}
