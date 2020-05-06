using Microsoft.EntityFrameworkCore.Migrations;
using System.Security.Cryptography;
using System.Text;

namespace restaurant_server.Migrations
{
    public partial class AddDefaultAdmin : Migration
    {
        byte[] password = SHA512.Create().ComputeHash(Encoding.UTF8.GetBytes("admin"));

        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.InsertData(table: "Users",
                columns: new string[] { "Name", "Password", "IsAdmin" },
                values: new object[] { "Admin", password, true }
            );
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DeleteData(table: "Users",
                keyColumns: new string[] { "Name", "Password", "IsAdmin" },
                keyValues: new object[] { "Admin", password, true }
            );
        }
    }
}
