using System.ComponentModel.DataAnnotations;

namespace LinkServer.Models
{
    /// <summary>
    /// 用于验证码服务提交或获取的模型
    /// </summary>
    public class Captcha
    {
        /// <summary>
        /// 验证码的Key，对于图形验证码，该值是验证码位于服务器的索引；对于邮箱验证码，该值为被验证的邮箱账号
        /// </summary>
        [Required]
        public string Key { get; set; }

        /// <summary>
        /// 从邮箱或者图形中人为获取到的验证码，该码应该与<see cref="Key"/>一起提交以供服务器验证
        /// </summary>
        [Required]
        public string Code { get; set; }
    }
}
