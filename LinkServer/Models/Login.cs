using LinkServer.Components;
using System.ComponentModel.DataAnnotations;

namespace LinkServer.Models
{
    /// <summary>
    /// 用于用户登录的请求模型
    /// </summary>
    public class Login
    {
        /// <summary>
        /// 用户名、用户ID、Email
        /// </summary>
        [Required(ErrorMessage = "用户名不可为空")]
        public string Name { get; set; }

        /// <summary>
        /// 用户密码
        /// </summary>
        [Required(ErrorMessage = "密码不可为空")]
        [RegularExpression(Utils.UserPasswordRegex, ErrorMessage = "密码格式不正确")]
        public string Password { get; set; }
    }
}