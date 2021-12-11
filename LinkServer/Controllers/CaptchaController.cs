using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Hei.Captcha;
using System.Net.Http;
using Microsoft.AspNetCore.Cors;
using LinkServer.Services;
using LinkServer.Models;
using System.ComponentModel.DataAnnotations;
using LinkServer.Components;

namespace LinkServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    [EnableCors("Any")]
    public partial class CaptchaController : ControllerBase
    {
        private readonly CaptchaService service;

        /// <summary>
        /// 使用<see cref="CaptchaService"/>构造控制器，由DI容器使用
        /// </summary>
        /// <param name="captchaService"></param>
        public CaptchaController(CaptchaService captchaService)
        {
            service = captchaService;
        }

        /// <summary>
        /// 获取图形验证码
        /// </summary>
        /// <returns></returns>
        [HttpGet("iamge")]
        public ImageCaptcha ImageCaptcha()
        {
            var (code, image, contentType) = service.CreateImageCaptcha();
            return new() { Key = code, Image = File(image, contentType) };
        }

        /// <summary>
        /// 验证图形验证码
        /// </summary>
        /// <param name="model"></param>
        /// <returns></returns>
        [HttpPost("verification/image")]
        public bool ImageCaptchaVerification([FromBody] Captcha model)
        {
            return service.VerifyImageCaptcha(model);
        }
    }
}
