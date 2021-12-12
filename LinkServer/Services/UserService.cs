using LinkServer.DAO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using LinkServer.Models;
using Microsoft.EntityFrameworkCore;
using System.ComponentModel.DataAnnotations;
using LinkServer.Components;
using System.Text.RegularExpressions;
using System.IO;

namespace LinkServer.Services
{

    /// <summary>
    /// 提供用户相关服务
    /// </summary>
    public class UserService
    {
        private readonly UserContext userContext;

        private readonly TokenService tokenService;

        private readonly CaptchaService captchaService;

        /// <summary>
        /// 务必使用依赖注入构造
        /// </summary>
        /// <param name="userContext"></param>
        /// <param name="tokenService"></param>
        /// <param name="captchaService"></param>
        public UserService(UserContext userContext, TokenService tokenService, CaptchaService captchaService)
        {
            this.userContext = userContext;
            this.tokenService = tokenService;
            this.captchaService = captchaService;
        }

        /// <summary>
        /// 注册并获取用户Token
        /// </summary>
        /// <param name="model"></param>
        /// <returns>注册成功后返回用户Token</returns>
        /// <exception cref="InvalidModelException"></exception>
        public string Signup(Signup model)
        {
            if (!IsAvailableName(model.Name))
                throw new InvalidModelException("名称不可用", nameof(model.Name));

            if(!IsAvailableEmail(model.Email))
                throw new InvalidModelException("邮箱不可用", nameof(model.Email));

            if (!captchaService.VerifyEmailCaptcha(model.Email, model.EmailAuthCode))
                throw new InvalidModelException("邮箱验证码错误", nameof(model.EmailAuthCode));

            User user = new()
            {
                Name = model.Name,
                Password = model.Password,
                Email = model.Email,
            };

            userContext.Add(user);

            userContext.SaveChanges();

            return Login(new Login { Name = model.Name, Password = model.Password});
        }

        /// <summary>
        /// 用户登录
        /// </summary>
        /// <param name="model"></param>
        /// <returns>用户token</returns>
        /// <exception cref="InvalidModelException"></exception>
        public string Login(Login model)
        {
            User user = userContext.Users.FirstOrDefault((user) => user.Name == model.Name || user.Email == model.Name);

            if (user is not null)
            {
                if (user.Password == model.Password)
                    return tokenService.CreateToken(user.Id);
                else
                    throw new InvalidModelException("密码不正确", nameof(model.Password));
            }

            if(!long.TryParse(model.Name, out long id))
                throw new InvalidModelException("用户不存在", nameof(model.Name));

            user = userContext.Find<User>(id);
            if (user is null)
                throw new InvalidModelException("用户不存在", nameof(model.Name));

            if (user.Password != model.Password)
                throw new InvalidModelException("密码不正确", nameof(model.Password));

            return tokenService.CreateToken(user.Id);
        }

        public async Task UpdateUserImage(long id, string suffix, System.IO.Stream imageStream)
        {
            //TODO:: 添加接收图像文件的功能。
        }

        /// <summary>
        /// 获取用户信息
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        public UserDTO User(long id)
        {
            return new UserDTO(userContext.Find<User>(id));
        }

        /// <summary>
        /// 为用户创建token
        /// </summary>
        /// <param name="id"></param>
        /// <returns></returns>
        /// <exception cref="InvalidModelException"></exception>
        public string CreateToken(long id)
        {
            if (userContext.Users.Find(id) is null)
                throw new InvalidModelException("用户不存在");

            return tokenService.CreateToken(id);
        }

        /// <summary>
        /// 更新用户信息
        /// </summary>
        /// <param name="model"></param>
        public void UpdateUser(UserDTO model)
        {
            var user = userContext.Find<User>(model.Id);
            user.TVName = model.TVName ?? model.TVName;

            // TODO:: 下次一定
            // user.Image = model.Image ?? model.Image;
            // user.TVImage = model.TVImage ?? model.TVImage;

            userContext.Update(user);
            userContext.SaveChanges();
        }

        /// <summary>
        /// 向未注册的邮箱发送验证码
        /// </summary>
        /// <param name="email"></param>
        /// <exception cref="InvalidModelException"></exception>
        public void SendGuestEmailAuthCode(string email)
        {
            if (!IsAvailableEmail(email))
                throw new InvalidModelException("该邮箱不可用", nameof(email));

            captchaService.SendEmailCaptcha(email);
        }

        /// <summary>
        /// 向指定用户的邮箱发送验证码
        /// </summary>
        /// <param name="id"></param>
        public void SendUserEmailAuthCode(long id)
        {
            var user = userContext.Users.Find(id);
            captchaService.SendEmailCaptcha(user.Email);
        }

        /// <summary>
        /// 判断邮箱是否已经被注册
        /// </summary>
        /// <param name="email"></param>
        /// <returns></returns>
        public bool IsAvailableEmail(string email)
        {
            return !userContext.Users.Any((user) => user.Email == email);
        }

        /// <summary>
        /// 判断用户名是否已经被注册
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public bool IsAvailableName(string name)
        {
            return !userContext.Users.Any((user) => user.Name == name);
        }

        /// <summary>
        /// 更新用户核心信息
        /// </summary>
        /// <param name="id"></param>
        /// <param name="model"></param>
        /// <exception cref="InvalidModelException"></exception>
        public void UpdateUserCore(long id, UserCore model)
        {
            User user = userContext.Find<User>(id);

            if (model.Name is not null && IsAvailableName(model.Name))
                user.Name = model.Name;

            if (model.NewEmail is not null && model.NewEmail.Any() &&
                model.NewEmailAuthCode is not null && model.NewEmailAuthCode.Any() &&
                !captchaService.VerifyEmailCaptcha(model.NewEmail, model.NewEmailAuthCode))
            {
                user.Email = model.NewEmail;
            }

            if (!captchaService.VerifyEmailCaptcha(model.Email, model.EmailAuthCode))
                throw new InvalidModelException("邮箱验证码错误", nameof(model.EmailAuthCode));

            user.Password = model.Password ?? user.Password;

            userContext.Update(user);
            userContext.SaveChanges();
        }
    }
}