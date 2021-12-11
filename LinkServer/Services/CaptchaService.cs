using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Security.Cryptography;
using System.Threading.Tasks;
using Hei.Captcha;
using LinkServer.Models;
using System.Text;
using Microsoft.Extensions.Caching.Distributed;
using LinkServer.Components;
using LinkServer.DAO;
using Microsoft.AspNetCore.Mvc;

namespace LinkServer.Services
{
    /// <summary>
    /// 提供图形、邮箱验证码服务
    /// </summary>
    public class CaptchaService
    {
        private readonly SecurityCodeHelper securityCodeHelper;

        private readonly EmailService emailService;

        private readonly IDistributedCache cache;

        
        public CaptchaService(SecurityCodeHelper codeHelper, EmailService emailService, ITempCache captchaCache)
        {
            securityCodeHelper = codeHelper;
            this.emailService = emailService;
            cache = captchaCache.DistributedCache;
        }

        private const string CacheKeyBeforce = nameof(CaptchaService) + ":Cache";

        /// <summary>
        /// 生成图形验证码
        /// </summary>
        /// <returns>验证码的唯一表示、验证码图像、图像类型</returns>
        public (string, byte[], string) CreateImageCaptcha()
        {
            var code = Utils.CreateRandomStr(4);
            var imageBytes = securityCodeHelper.GetEnDigitalCodeByte(code);

            var key = Utils.CreateRandomStr(16);

            cache.SetString($"{CacheKeyBeforce}:Image:{key}", code, new()
            {
                AbsoluteExpirationRelativeToNow = new TimeSpan(0, 1, 0)
            });

            return (key, imageBytes, "image/png");
        }

        /// <summary>
        /// 发送验证码到指定邮箱
        /// </summary>
        /// <param name="email"></param>
        /// <exception cref="InvalidModelException"></exception>
        public void SendEmailCaptcha(string email)
        {
            if (cache.GetString($"{CacheKeyBeforce}:Email:{email}") != null)
                throw new InvalidModelException("邮箱验证码的请求间隔为2分钟", nameof(email));

            var code = Utils.CreateRandomStr(6);

            var emailContnet = $"您正在向Link Live请求一个验证，这儿是您的验证码，该验证码2分钟之内有效：<br><strong>{code}</strong>";
            var emailTitle = "Link live验证";
            var emailTask = emailService.SendToAsync(email, emailContnet, emailTitle);

            cache.SetString($"{CacheKeyBeforce}:Email:{email}", code, new()
            {
                AbsoluteExpirationRelativeToNow = new TimeSpan(0, 2, 0)
            });
            
            emailTask.Wait();
        }

        /// <summary>
        /// 验证邮箱验证码
        /// </summary>
        /// <param name="email"></param>
        /// <param name="authCode"></param>
        /// <returns>成功返回true，失败返回false</returns>
        public bool VerifyEmailCaptcha(string email, string authCode)
        {
            if (cache.GetString($"{CacheKeyBeforce}:Email:{email}") != authCode)
                return false;

            cache.Remove($"{CacheKeyBeforce}:Email:{email}");

            return true;
        }

        /// <summary>
        /// 验证图形验证码的邮箱性，如果无效则抛出<see cref="InvalidModelException"/>异常
        /// </summary>
        /// <param name="model">验证码标识和用户输入的验证码模型</param>
        /// <returns>成功返回true，否则false</returns>
        public bool VerifyImageCaptcha(Captcha model)
        {
            if (cache.GetString($"{CacheKeyBeforce}:Image:{model.Key}") != model.Code)
                return false;

            cache.Remove($"{CacheKeyBeforce}:Image:{model.Key}");

            return true;
        }
    }
}