using LinkServer.Components;
using System.ComponentModel.DataAnnotations;

namespace LinkServer.Models
{
    /// <summary>
    /// 用于客户端注册请求的模型
    /// </summary>
    public class Signup
    {
        /// <summary>
        /// 用户名，该用户名必须未被注册
        /// </summary>
        [Required(ErrorMessage = "用户名不可为空")]
        [RegularExpression(
            Utils.UserNameRegex, 
            ErrorMessage = "必须以英文字母或中文开头，只允许使用中文、英文字母、数字、下划线，长度在6-64之间")]
        public string Name { get; set; }

        /// <summary>
        /// 邮箱，该邮箱必须未被注册
        /// </summary>
        [Required(ErrorMessage = "邮箱不可为空")]
        [EmailAddress(ErrorMessage = "邮箱格式不正确")]
        public string Email { get; set; }

        /// <summary>
        /// 邮箱验证码
        /// </summary>
        [Required(ErrorMessage = "邮箱验证码不可为空")]
        public string EmailAuthCode { get; set; }

        /// <summary>
        /// 用户密码
        /// </summary>
        [Required(
            ErrorMessage = "密码不可为空")]
        [RegularExpression(
            Utils.UserPasswordRegex, 
            ErrorMessage = "由数字和字母组成，并且要同时含有数字和字母，且长度要在8-16位之间")]
        public string Password { get; set; }
    }
}
