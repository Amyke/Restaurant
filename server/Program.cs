using communication_lib;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using restaurant_server.Persistence;
using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

namespace restaurant_server
{
    class Program
    {
        class RestaurantApplication : IHostedService
        {
            ILoggerFactory _loggerFactory;
            ILogger<RestaurantApplication> _logger;
            IServiceProvider _services;
            IHostApplicationLifetime _lifetime;
            ConnectionHandler? _connectionHandler;

            public UInt16 Port { get; private set; }

            public RestaurantApplication(IConfiguration configuration, IServiceProvider services, IHostApplicationLifetime lifetime)
            {
                _services = services;
                _loggerFactory = _services.GetRequiredService<ILoggerFactory>();
                _logger = _loggerFactory.CreateLogger<RestaurantApplication>();
                _lifetime = lifetime;
                if (UInt16.TryParse(configuration["port"], out var port))
                {
                    Port = port;
                }
                else
                {
                    _logger.LogCritical("Can not start: {} is not a valid port number", configuration["port"]);
                    _lifetime.StopApplication();
                }
            }

            public async Task StartAsync(CancellationToken cancellationToken)
            {
                if (_lifetime.ApplicationStopping.IsCancellationRequested)
                {
                    return;
                }
                using (var db = _services.GetRequiredService<RestaurantContext>())
                {
                    await db.Database.MigrateAsync();
                }

                var comm = new Communication(new IPEndPoint(IPAddress.Any, Port));
                var model = new Model(() => _services.GetRequiredService<RestaurantContext>());
                _connectionHandler = new ConnectionHandler(comm, model, cancellationToken, _loggerFactory.CreateLogger<ConnectionHandler>());
                try
                {
                    _logger.LogInformation("Listening...");
                    await comm.Listen(cancellationToken);
                }
                catch (OperationCanceledException)
                {
                    _logger.LogInformation("Listen finished");
                }
                catch (Exception e)
                {
                    _logger.LogCritical(e, "Unhandled exception");
                }
            }

            public async Task StopAsync(CancellationToken cancellationToken)
            {
                if (_connectionHandler != null)
                {
                    await _connectionHandler.DisposeAsync();
                }
                _logger.LogInformation("Goodbye cruel world");
            }
        }

        static async Task Main(string[] args)
        {
            await CreateHostBuilder(args).RunConsoleAsync();
        }

        public static IHostBuilder CreateHostBuilder(string[] args)
        {
            return Host.CreateDefaultBuilder(args)
                .ConfigureAppConfiguration(builder =>
                {
                    builder.AddInMemoryCollection(new Dictionary<string, string>
                    {
                        { "port", "9007" },
                        { "databaseConnectionString", "Host=localhost;Port=5432;Database=restaurant;Username=postgres;Password=admin" }
                    });
                    string configBase;
                    if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
                    {
                        configBase = "/etc";
                    }
                    else
                    {
                        configBase = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData, Environment.SpecialFolderOption.Create);
                    }
                    configBase = Path.Join(configBase, "restaurant");
                    Directory.CreateDirectory(configBase);
                    builder.SetBasePath(configBase);
                    builder.AddJsonFile("server.json", optional: true);
                })
                .ConfigureServices((context, services) =>
                {
                    services.AddLogging(builder =>
                    {
                        builder.AddConsole(console =>
                        {
                            console.IncludeScopes = false;
                            console.TimestampFormat = "[HH:mm:ss] ";
                        });
                        //builder.AddFile();
                        builder.SetMinimumLevel(LogLevel.Debug);
                    });
                    services.AddDbContext<RestaurantContext>(options =>
                    {
                        options.UseNpgsql(context.Configuration["databaseConnectionString"]);
                    }, ServiceLifetime.Transient);
                    services.AddHostedService<RestaurantApplication>();
                });
        }
    }
}
