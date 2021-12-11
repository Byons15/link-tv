using Microsoft.AspNetCore.Mvc;

namespace LinkServer.Models
{
    /// <summary>
    /// 用于Nginx Rtmp服务器在推流时使用的验证模型，普通用户不应使用这个模型
    /// </summary>
    public class NginxStream
    {
        /// <summary>
        /// nginx回调时附带的推流源地址
        /// </summary>
        [ModelBinder(Name = "addr")]
        public string Addr { get; set; }

        /// <summary>
        /// 推流应用
        /// </summary>
        [ModelBinder(Name = "app")]
        public string App { get; set; }

        /// <summary>
        /// 流ID，每条流的唯一标识，应该是推流者的用户ID
        /// </summary>
        [ModelBinder(Name = "name")]
        public string StreamId { get; set; }

        /// <summary>
        /// 推流携带的验证参数
        /// </summary>
        [ModelBinder(Name = "txSecret")]
        public string Secret { get; set; }

        /// <summary>
        /// 推流携带的超时参数
        /// </summary>
        [ModelBinder(Name = "txTime")]
        public string ExpireTime { get; set; }
    }
}
