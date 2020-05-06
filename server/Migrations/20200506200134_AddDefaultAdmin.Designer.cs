﻿// <auto-generated />
using System;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Migrations;
using Microsoft.EntityFrameworkCore.Storage.ValueConversion;
using Npgsql.EntityFrameworkCore.PostgreSQL.Metadata;
using restaurant_server.Persistence;

namespace restaurant_server.Migrations
{
    [DbContext(typeof(RestaurantContext))]
    [Migration("20200506200134_AddDefaultAdmin")]
    partial class AddDefaultAdmin
    {
        protected override void BuildTargetModel(ModelBuilder modelBuilder)
        {
#pragma warning disable 612, 618
            modelBuilder
                .HasAnnotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn)
                .HasAnnotation("ProductVersion", "3.1.3")
                .HasAnnotation("Relational:MaxIdentifierLength", 63);

            modelBuilder.Entity("restaurant_server.Persistence.Food", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("integer")
                        .HasAnnotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn);

                    b.Property<string>("Name")
                        .IsRequired()
                        .HasColumnType("text");

                    b.Property<int>("Price")
                        .HasColumnType("integer");

                    b.Property<bool>("Visible")
                        .HasColumnType("boolean");

                    b.HasKey("Id");

                    b.ToTable("Foods");
                });

            modelBuilder.Entity("restaurant_server.Persistence.FoodAmount", b =>
                {
                    b.Property<int>("FoodId")
                        .HasColumnType("integer");

                    b.Property<int>("Amount")
                        .HasColumnType("integer");

                    b.HasKey("FoodId");

                    b.ToTable("FoodAmounts");
                });

            modelBuilder.Entity("restaurant_server.Persistence.Order", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("integer")
                        .HasAnnotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn);

                    b.Property<DateTime>("Date")
                        .HasColumnType("timestamp without time zone");

                    b.Property<int>("Status")
                        .HasColumnType("integer");

                    b.Property<int>("TableId")
                        .HasColumnType("integer");

                    b.HasKey("Id");

                    b.HasIndex("TableId");

                    b.ToTable("Orders");
                });

            modelBuilder.Entity("restaurant_server.Persistence.OrderFoodAmount", b =>
                {
                    b.Property<int>("OrderId")
                        .HasColumnType("integer");

                    b.Property<int>("FoodId")
                        .HasColumnType("integer");

                    b.Property<int>("Amount")
                        .HasColumnType("integer");

                    b.HasKey("OrderId", "FoodId");

                    b.HasIndex("FoodId");

                    b.ToTable("OrderFoodAmounts");
                });

            modelBuilder.Entity("restaurant_server.Persistence.User", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd()
                        .HasColumnType("integer")
                        .HasAnnotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn);

                    b.Property<bool>("IsAdmin")
                        .HasColumnType("boolean");

                    b.Property<string>("Name")
                        .IsRequired()
                        .HasColumnType("text");

                    b.Property<byte[]>("Password")
                        .IsRequired()
                        .HasColumnType("bytea");

                    b.HasKey("Id");

                    b.ToTable("Users");
                });

            modelBuilder.Entity("restaurant_server.Persistence.FoodAmount", b =>
                {
                    b.HasOne("restaurant_server.Persistence.Food", "Food")
                        .WithMany()
                        .HasForeignKey("FoodId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();
                });

            modelBuilder.Entity("restaurant_server.Persistence.Order", b =>
                {
                    b.HasOne("restaurant_server.Persistence.User", "Table")
                        .WithMany()
                        .HasForeignKey("TableId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();
                });

            modelBuilder.Entity("restaurant_server.Persistence.OrderFoodAmount", b =>
                {
                    b.HasOne("restaurant_server.Persistence.Food", "Food")
                        .WithMany()
                        .HasForeignKey("FoodId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();

                    b.HasOne("restaurant_server.Persistence.Order", "Order")
                        .WithMany()
                        .HasForeignKey("OrderId")
                        .OnDelete(DeleteBehavior.Cascade)
                        .IsRequired();
                });
#pragma warning restore 612, 618
        }
    }
}
