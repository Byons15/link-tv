using Microsoft.AspNetCore.Mvc;

namespace LinkServer.Models
{
    public class TencentLiveStream
    {
        public string app { get; set; }

        public string stream_id { get; set; }

        public string stream_param { get; set; }
    }
}
