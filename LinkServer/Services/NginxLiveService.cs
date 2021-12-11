using LinkServer.Components;
using LinkServer.Models;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;
using System;
using System.Net;

namespace LinkServer.Services
{
    public class NginxLiveService : ILiveServer
    {
        private class NginxRTMPServerOptions
        {
            public string Host { get; set; }
            public int PushPort { get; set; }
            public int ControlPort { get; set; }
            public string AppName { get; set; }
        }

        private readonly NginxRTMPServerOptions options = new();

        public string Host { get => $"{options.Host}:{options.PushPort}"; }

        public string AppName { get => options.AppName; }

        public NginxLiveService(IConfiguration configuration)
        {
            configuration.GetSection("NginxRTMPServerOptions").Bind(options);
        }

        public void DropStream(string streamId)
        {
            throw new NotImplementedException();
        }

        public void OnAuthorizationKeyChanged(string masterKey, string backupKey)
        {
            // pass
        }

        public string HLSPlayAddress(string streamId)
        {
            return $"http://{options.Host}:{options.ControlPort}/{options.AppName}/{streamId}.m3u8";
        }

        public string FLVPlayAddress(string streamId)
        {
            return $"http://{options.Host}:{options.ControlPort}/{options.AppName}?app={options.AppName}&stream={streamId}";
        }
    }
}
