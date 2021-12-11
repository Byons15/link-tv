using Microsoft.AspNetCore.Mvc;

namespace LinkServer.Models
{
    /// <summary>
    /// 用于客户端请求图形验证码的响应模型
    /// </summary>
    public class ImageCaptcha
    {
        /// <summary>
        /// 唯一标识
        /// </summary>
        public string Key { get; set; }

        /// <summary>
        /// base64编码图片
        /// </summary>
        public FileContentResult Image { get; set; }
    }
}