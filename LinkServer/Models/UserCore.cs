using System.ComponentModel.DataAnnotations;
using LinkServer.Components;

namespace LinkServer.Models
{
    /// <summary>
    /// 描述用户核心信息的模型，用于获取或修改用户核心信息，用于修改用户信息时，必须总是附带<see cref="EmailAuthCode"/>，
    /// 要修改邮箱时，同时也要带上新邮箱的验证许可Key
    /// </summary>
    public class UserCore
    {
        /// <summary>
        /// 邮箱，
        /// </summary>
        [EmailAddress(ErrorMessage = "邮箱格式不正确")]
        public string Email { get; set; }

        /// <summary>
        /// 用户名
        /// </summary>
        [RegularExpression(Utils.UserNameRegex, ErrorMessage = "必须以英文字母或中文开头，只允许使用中文、英文字母、数字、下划线，长度在6-64之间")]
        public string Name { get; set; }

        /// <summary>
        /// 要修改用户信息时，必须同时提交Email的验证许可Key，否则修改会无效,从服务端获取时总是为空
        /// </summary>
        [Required(ErrorMessage = "邮箱许可不能为空")]
        public string EmailAuthCode { get; set; }

        /// <summary>
        /// 要修改的新邮箱，从服务端获取时总是为空
        /// </summary>
        [EmailAddress(ErrorMessage = "邮箱格式不正确")]
        public string NewEmail { get; set; }

        /// <summary>
        /// 修改邮箱时，新邮箱的验证许可Key，从服务端获取时总是为空
        /// </summary>
        public string NewEmailAuthCode { get; set; }

        /// <summary>
        /// 用户密码，从服务端获取时总是为空。
        /// </summary>
        [RegularExpression(Utils.UserPasswordRegex, ErrorMessage = "由数字和字母组成，并且要同时含有数字和字母，且长度要在8-16位之间")]
        public string Password { get; set; }

        /// <summary>
        /// 默认构造
        /// </summary>
        public UserCore()
        {
            // pass
        }

        /// <summary>
        /// 使用User构造
        /// </summary>
        /// <param name="user"></param>
        public UserCore(User user)
        {
            Email = user.Email;
            Name = user.Name;
        }
    }
}
