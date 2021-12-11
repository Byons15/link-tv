using LinkServer.Components;
using LinkServer.DAO;
using LinkServer.Models;
using System;
using System.Linq;
using System.Collections.Generic;
using System.Security.Cryptography;
using Microsoft.Extensions.Configuration;

namespace LinkServer.Services
{
    public class LiveService
    {
        private readonly TokenService tokenService;
        private readonly UserContext userContext;
        private readonly ILiveServer liveServer;

        private class Options
        {
            public string SecretMasterKey { get; set; }

            public string SecretBackupKey { get; set; }
        }

        private Options options = new();

        public LiveService(TokenService tokenService, UserContext userContext, ILiveServer liveServer, IConfiguration configuration)
        {
            this.tokenService = tokenService;
            this.userContext = userContext;
            this.liveServer = liveServer;

            configuration.GetSection("LiveServiceOptions").Bind(options);
        }

        public string PushAddress(long streamId)
        {
            DateTime unixStartingTime = new(1970, 1, 1);
            var expireTime = (ulong)((DateTime.Now.AddDays(1) - unixStartingTime).TotalSeconds);
            string HEXExpireTime = expireTime.ToString("X").ToUpper();
            return $"rtmp://{liveServer.Host}/{liveServer.AppName}/{streamId}?txSecret={CreateSecret(streamId.ToString(), options.SecretMasterKey, HEXExpireTime)}&txTime={HEXExpireTime}";
        }

        public string HLSPlayAddress(long streamId)
        {
            return liveServer.HLSPlayAddress(streamId.ToString());
        }

        public string FLVPlayAddress(long streamId)
        {
            return liveServer.FLVPlayAddress(streamId.ToString());
        }

        private string CreateSecret(string streamId, string authKey, string expireTime)
        {
            string secret = authKey + streamId.ToString() + expireTime;
            using (var md5 = MD5.Create())
            {
                secret = BitConverter.ToString(md5.ComputeHash(System.Text.Encoding.ASCII.GetBytes(secret))).Replace("-", "");
            }

            return secret;
        }

        public void Push(Stream model)
        {
            if(model.TxSecret != CreateSecret(model.StreamId, options.SecretMasterKey, model.TxTime))
                liveServer.DropStream(model.StreamId);

            long id = long.Parse(model.StreamId);
            var user = userContext.Users.Find(id);
            Studio studio = new()
            {
                Id = user.Id,
                Director = user.Name,
                Name = user.TVName ?? $"{user.Name}的直播间",
            };

            if (userContext.Studios.Find(id) is null)
                userContext.Studios.Add(studio);

            userContext.SaveChanges();
        }

        public Studio Find(string name)
        {
            var result = from s in userContext.Studios
                         where s.Director == name || s.Id.ToString() == name
                         select s;

            int count = result.Count();

            if (!result.Any())
                throw new InvalidModelException("没有找到直播间");

            var studio = result.First();
            studio.HlsAddress = HLSPlayAddress(studio.Id);
            studio.FlvAddress = FLVPlayAddress(studio.Id);

            return studio;
        }

        public void PushDone(Stream model)
        {
            userContext.Studios.Remove(userContext.Studios.Find(long.Parse(model.StreamId)));
            userContext.SaveChanges();
        }

        public IEnumerable<Studio> All()
        {
            var studios = userContext.Studios.ToList();
            foreach(var studio in studios)
            {
                studio.FlvAddress = FLVPlayAddress(studio.Id);
                studio.HlsAddress = HLSPlayAddress(studio.Id);
            }

            return studios;
        }
    }
}
