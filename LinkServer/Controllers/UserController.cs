using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Cors;
using LinkServer.Components;
using LinkServer.Models;
using LinkServer.Services;
using LinkServer.DAO;
using System;
using System.ComponentModel.DataAnnotations;
using Microsoft.AspNetCore.Http;
using System.IO;
using System.Threading.Tasks;

namespace LinkServer.Controllers
{
    /// <summary>
    /// 用户相关控制器
    /// </summary>
    [Authorize]
    [Route("api/[controller]")]
    [ApiController]
    [EnableCors("Any")]
    public class UserController : ControllerBase
    {
        private readonly UserService userService;

        /// <summary>
        /// 构造
        /// </summary>
        /// <param name="userService"></param>
        public UserController(UserService userService)
        {
            this.userService = userService;
        }

        /// <summary>
        /// 注册并登录
        /// </summary>
        /// <param name="model">注册请求模型</param>
        /// <returns>token</returns>
        [HttpPost("signup")]
        [AllowAnonymous]
        public string Signup([FromBody] Signup model)
        {
            return userService.Signup(model);
        }

        /// <summary>
        /// 登录
        /// </summary>
        /// <param name="model"></param>
        /// <returns>token</returns>
        [HttpPost("login")]
        [AllowAnonymous]
        public string Login([FromBody]Login model)
        {
            return userService.Login(model);
        }

        /// <summary>
        /// 刷新token
        /// </summary>
        /// <param name="id"></param>
        /// <returns>新的token</returns>
        [HttpGet("token")]
        [UserAction]
        public string RefreshToken(long id)
        {
            return userService.CreateToken(id);
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        /// <param name="id"></param>
        /// <returns>用户信息</returns>
        [HttpGet]
        [UserAction]
        public async Task<UserDTO> Get(long id)
        {
            return await userService.UserAsync(id);
        }

        /// <summary>
        /// 验证用户名是否可用
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        [HttpGet("valid-name")]
        [AllowAnonymous]
        public bool ValidName([RegularExpression(Utils.UserNameRegex, ErrorMessage = "必须以英文字母或中文开头，只允许使用中文、英文字母、数字、下划线，长度在6-64之间")] string name)
        {
            return userService.IsAvailableName(name);
        }

        /// <summary>
        /// 验证邮箱是否可用
        /// </summary>
        /// <param name="email"></param>
        /// <returns></returns>
        [HttpGet("valid-email")]
        [AllowAnonymous]
        public bool ValidEmail([EmailAddress(ErrorMessage = "邮箱格式不正确")]string email)
        {
            return userService.IsAvailableEmail(email);
        }

        /// <summary>
        /// 向未注册的邮箱发送验证码
        /// </summary>
        /// <param name="email"></param>
        [HttpGet("guest-email-auth-code")]
        [AllowAnonymous]
        public void SendGuestEmailAuthCode([EmailAddress(ErrorMessage = "邮箱格式不正确")] string email)
        {
            userService.SendGuestEmailAuthCode(email);
        }

        /// <summary>
        /// 向用户的邮箱发送验证码。
        /// </summary>
        /// <param name="id"></param>
        [HttpGet("user-email-auth-code")]
        [UserAction]
        public void SendUserEmailAuthCode(long id)
        {
            userService.SendUserEmailAuthCode(id);
        }

        /// <summary>
        /// 用户核心信息更改
        /// </summary>
        /// <param name="id"></param>
        /// <param name="model"></param>
        [HttpPost("core-alteration")]
        [UserAction]
        public void UserCoreAlteration(long id, [FromBody]UserCore model)
        {
            userService.UpdateUserCore(id, model);
        }

        /// <summary>
        /// 用户信息更改
        /// </summary>
        /// <param name="id"></param>
        /// <param name="model"></param>
        [HttpPost("alteration")]
        [UserAction]
        public void UserAlteration(long id, [FromBody]UserDTO model)
        {
            model.Id = id;
            userService.UpdateUser(model);
        }

        [HttpPost("userImage")]
        [UserAction]
        public async Task UpdateUserImage(long id, 
            [Required(ErrorMessage = "必须提供文件名后缀")]
            [StringLength(8, MinimumLength = 2, ErrorMessage = "文件名后缀不合法")]string suffix, 
            [Required(ErrorMessage = "必须提供文件")]IFormFile imageFile)
        {
            await userService.UpdateUserImageAsync(id, suffix, imageFile.OpenReadStream());
        }
    }
}
