using Microsoft.Extensions.Configuration;
using System;
using System.Threading.Tasks;
using TencentCloud.Common;
using TencentCloud.Common.Profile;
using TencentCloud.Live.V20180801;
using TencentCloud.Live.V20180801.Models;

namespace LinkServer.Services
{
    public class TencentLiveService : ILiveServer
    {
        private class CallbackTemplateOptions
        {
            public string CallbackKey { get; set; }
            public int Id { get; set; }
        }

        CallbackTemplateOptions callbackTemplateOptions = new();

        private readonly Credential credential = new();

        private readonly string liveCallbackNotifyUrl;

        private readonly string pushDomain;

        public TencentLiveService(IConfiguration configuration)
        {
            var tencentLiveOptions = configuration.GetSection("TencentLiveOptions");
            tencentLiveOptions.GetSection("Credential").Bind(credential);
            tencentLiveOptions.GetSection("CallbackTemplate").Bind(callbackTemplateOptions);
            pushDomain = tencentLiveOptions["PushDomain"];
            liveCallbackNotifyUrl = configuration["LaunchOptions:AccessUrl"];

            _ = UpdateLiveCallbackTemplate();
        }

        private async Task UpdateLiveCallbackTemplate()
        {
            await Task.Run(() =>
            {
                ClientProfile clientProfile = new();
                HttpProfile httpProfile = new();
                httpProfile.Endpoint = "live.tencentcloudapi.com";
                clientProfile.HttpProfile = httpProfile;

                LiveClient client = new(credential, "", clientProfile);
                ModifyLiveCallbackTemplateRequest req = new();
                req.TemplateId = callbackTemplateOptions.Id;
                req.StreamBeginNotifyUrl = liveCallbackNotifyUrl + "/api/live/tencentStreamPush";
                req.StreamEndNotifyUrl = liveCallbackNotifyUrl + "/api/live/TencentStreamPushDone";
                req.CallbackKey = callbackTemplateOptions.CallbackKey;
                ModifyLiveCallbackTemplateResponse resp = client.ModifyLiveCallbackTemplateSync(req);
                Console.WriteLine(resp.RequestId);
            });
        }

        public string Host => pushDomain;

        public string AppName => "live";

        public void DropStream(string streamId)
        {
            
        }

        public string FLVPlayAddress(string streamId)
        {
            return $"http://liveplay.byons.site/live/{streamId}.flv";
        }

        public string HLSPlayAddress(string streamId)
        {
            return $"http://liveplay.byons.site/live/{streamId}.m3u8";
        }

        public void OnAuthorizationKeyChanged(string masterKey, string backupKey)
        {
            throw new NotImplementedException();
        }
    }
}
