using communication_lib;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using restaurant_server.Persistence;
using System;
using System.Net;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{
    class Program
    {
        static async Task Main(string[] args)
        {
            using var host = await CreateHostBuilder(args).StartAsync();
            var loggerFactory = host.Services.GetRequiredService<ILoggerFactory>();
            var logger = loggerFactory.CreateLogger<Program>();

            logger.LogInformation("Hello World!");

            var dbContext = host.Services.GetRequiredService<RestaurantContext>();
            await dbContext.Database.MigrateAsync();

            //using (var trx = await dbContext.Database.BeginTransactionAsync())
            //{
            //    dbContext.AddRangeAsync(new User { }, new Food { });
            //}

            var comm = new Communication(new IPEndPoint(IPAddress.Any, 9007));
            CancellationTokenSource tokenSource = new CancellationTokenSource();
            CancellationToken _cancellation = tokenSource.Token;
            Model model = new Model(dbContext);
            await using (var connectionHandler = new ConnectionHandler(comm, model, _cancellation))
            {
                Console.CancelKeyPress += (s, args) =>
                {
                    tokenSource.Cancel();
                    args.Cancel = true;
                };
                try
                {
                    logger.LogInformation("Listening...");
                    await comm.Listen(_cancellation);
                }
                catch (OperationCanceledException)
                {
                    logger.LogInformation("Listen finished");
                }
                catch(Exception e)
                {
                    logger.LogCritical(e, "Unhandled exception");
                }
            }

            logger.LogInformation("Goodbye cruel world");
        }

        public static IHostBuilder CreateHostBuilder(string[] args)
        {
            return Host.CreateDefaultBuilder(args).ConfigureServices(services =>
            {
                services.AddDbContext<RestaurantContext>(options =>
                {
                    options.UseNpgsql("Host=localhost;Port=5432;Database=restaurant;Username=postgres;Password=admin");
                });
                services.AddLogging(builder =>
                {
                    builder.AddConsole();
                });
            });
        }
    }
}
