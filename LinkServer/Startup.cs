using Hei.Captcha;
using LinkServer.DAO;
using LinkServer.Models;
using LinkServer.Services;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.IdentityModel.Tokens;
using System;
using System.Text;
using Microsoft.Extensions.Caching.StackExchangeRedis;
using LinkServer.Components;
using System.IO;
using Microsoft.AspNetCore.Mvc;
using System.Linq;
using NSwag.CodeGeneration.TypeScript;
using NSwag;
using NSwag.Generation.Processors.Security;
using Newtonsoft.Json.Serialization;
using LinkServer.Hubs;
using System.Threading.Tasks;
using Microsoft.AspNetCore.SignalR;

namespace LinkServer
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddCors(options =>
            {
                options.AddPolicy("Any", builder =>
                {
                    builder.SetIsOriginAllowed(_ => true).AllowAnyMethod().AllowAnyHeader().AllowCredentials();
                });
            });
            var tokenOptions = Configuration.GetSection("TokenServiceOptions");
                services.AddAuthentication(auth =>
                {
                    auth.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
                    auth.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
                })
                .AddJwtBearer(token =>
                {
                    token.RequireHttpsMetadata = false;
                    token.SaveToken = true;
                    token.TokenValidationParameters = new TokenValidationParameters
                    {
                        ValidateIssuerSigningKey = true,
                        IssuerSigningKey = new SymmetricSecurityKey(Encoding.ASCII.GetBytes(tokenOptions["SecurityKey"])),
                        ValidateIssuer = true,
                        ValidIssuer = tokenOptions.GetValue<string>("Issuer"),
                        ValidateAudience = true,
                        ValidAudience = tokenOptions.GetValue<string>("Audience"),
                        RequireExpirationTime = true,
                        ValidateLifetime = true,
                        ClockSkew = TimeSpan.Zero
                    };

                    token.Events = new JwtBearerEvents
                    { 
                        OnMessageReceived = (context) => {
                            if (!context.HttpContext.Request.Path.HasValue)
                            {
                                return Task.CompletedTask;
                            }
                            var accessToken = context.HttpContext.Request.Query["access_token"];
                            var path = context.HttpContext.Request.Path;
                            if (!(string.IsNullOrWhiteSpace(accessToken)) && path.StartsWithSegments("/chat"))
                            {
                                context.Token = accessToken;
                                return Task.CompletedTask;
                            }
                            return Task.CompletedTask;
                        }
                    };
                });

            services.AddSignalR();

            services.AddControllers(options =>
            {
                options.Filters.Add<ExceptionFilter>();
            });

            #region Swashbuckle
            // 以后添加
            // services.AddInvalidModelResponseExtensions();

            //services.AddSwaggerGen(c =>
            //{
            //    c.SwaggerDoc("v1", new OpenApiInfo { Title = "UsersServer", Version = "v1" });
            //    c.AddSecurityDefinition("Bearer", new OpenApiSecurityScheme
            //    {
            //        Name = "Authorization",
            //        In = ParameterLocation.Header,
            //        Type = SecuritySchemeType.ApiKey,
            //        BearerFormat = "JWT",
            //        Scheme = "Bearer"
            //    });

            //    c.AddSecurityRequirement(new OpenApiSecurityRequirement
            //    {
            //        {
            //            new OpenApiSecurityScheme
            //            {
            //                Reference = new OpenApiReference
            //                {
            //                    Type = ReferenceType.SecurityScheme,
            //                    Id = "Bearer"
            //                }
            //            } ,
            //            Array.Empty<string>()
            //        }
            //    });

            //    c.EnableAnnotations();

            //    c.AddServer(new () 
            //    { 
            //        Description = Configuration["LaunchOptions:Description"],
            //        Url = Configuration["LaunchOptions:AccessUrl"]
            //    });
            //    c.IncludeXmlComments(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "LinkServer.xml"), true);
            //})
            #endregion

            services.AddOpenApiDocument(config =>
            {
                config.PostProcess = document =>
                {
                    document.Info.Version = "0.0.1";
                    document.Info.Title = "LinkServer";
                    document.Info.Description = "为Link TV提供后端服务。";
        };

                config.AddSecurity("JWT", Enumerable.Empty<string>(), new OpenApiSecurityScheme
                {
                    Type = OpenApiSecuritySchemeType.ApiKey,
                    Name = "Authorization",
                    In = OpenApiSecurityApiKeyLocation.Header,
                    Description = "Type into the textbox: Bearer {your JWT token}."
                });

                config.OperationProcessors.Add(
                    new AspNetCoreOperationSecurityScopeProcessor("JWT"));

                config.DocumentProcessors.Add(new ResponseProcessor());
            });

            services.AddDbContext<UserContext>(options =>
             {
                 var connectionString = Configuration.GetConnectionString("UserDataBaseConnection");
                 options.UseMySQL(connectionString);
             })
            .AddHeiCaptcha()
            .AddTransient<CaptchaService>()
            .AddTransient<TokenService>()
            .AddTransient<EmailService>()
            .AddTransient<UserService>()
            .AddSingleton<ITempCache>(new TempCache(Configuration.GetConnectionString("TempCacheRedisConnection")))
            .AddTransient<LiveService>()
            .AddSingleton<ILiveServer, TencentLiveService>();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
                
            }
            // app.UseHttpsRedirection();

            app.UseRouting();
            app.UseCors("Any");
            app.UseAuthentication();
            app.UseAuthorization();

            #region UserSwashbuckle
            //app.UseSwagger();
            //app.UseSwaggerUI(c => c.SwaggerEndpoint("/swagger/v1/swagger.json", "LinkServer v1")); 
            #endregion
            app.UseOpenApi(config =>
            {
                config.PostProcess = (doc, httpReq) =>
                {
                    doc.Servers.Clear();
                    doc.Servers.Add(new OpenApiServer()
                    {
                        Url = Configuration["LaunchOptions:AccessUrl"],
                        Description = Configuration["LaunchOptions:Description"],
                    });
                };
            }); // serve OpenAPI/Swagger documents
            app.UseSwaggerUi3(); // serve Swagger UI

            // 在开发环境中生成客户端ts源码
            if (env.IsDevelopment())
            {
                _ = app.UseTypeScriptClientGeneratorAsync(Configuration["LaunchOptions:LoopBackUrl"] + "/swagger/v1/swagger.json");
            }

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapHub<ChatHub>("/ChatHub");
                endpoints.MapControllers();
            });
        }
    }
}
