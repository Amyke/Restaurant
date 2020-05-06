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

    class User
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

    class Food
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

    class FoodAmount
    {
        [Required]
        public Int32 FoodId { get; set; }
        [ForeignKey(nameof(FoodId))]
        public Food Food { get; set; }

        [Required]
        public int Amount { get; set; }
    }

    enum DbOrderStatus
    {
        Pending = 0,
        InProgress = 1,
        Completed = 2,
        Payed = 3
    }

    class Order
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        public Int32 Id { get; set; }

        [Required]
        public User Table { get; set; }

        [Required]
        public DateTime Date { get; set; }

        [Required]
        public DbOrderStatus Status { get; set; }

        [Required]
        IEnumerable<OrderFoodAmount> Foods { get; set; }
    }

    class OrderFoodAmount
    {
        [Required]
        public Int32 OrderId { get; set; }
        [ForeignKey(nameof(OrderId))]
        public Order Order { get; set; }

        [Required]
        public Int32 FoodId { get; set; }
        [ForeignKey(nameof(FoodId))]
        public Food Food { get; set; }

        [Required]
        public int Amount { get; set; }
    }
}
