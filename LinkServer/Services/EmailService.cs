using LinkServer.Components;
using LinkServer.Models;
using Microsoft.Extensions.Caching.Distributed;
using Microsoft.Extensions.Options;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Mail;
using System.Threading.Tasks;
using LinkServer.DAO;
using Microsoft.Extensions.Configuration;

namespace LinkServer.Services
{
    public class EmailService
    {
        class Options
        {
            public string Sender { get; set; }
            public string Password { get; set; }
            public string DisplayName { get; set; }
            public string Host { get; set; }
        }

        private readonly Options options = new();

        public EmailService(IConfiguration configuration)
        {
            configuration.GetSection("EmailServiceOptions").Bind(options);
        }

        private (SmtpClient, MailMessage) CreateMail(string receiver, string content, string title)
        {
            var mail = new MailMessage();

            mail.To.Add(receiver);

            mail.From = new MailAddress(options.Sender, options.DisplayName, System.Text.Encoding.UTF8);
            mail.Subject = title;
            mail.SubjectEncoding = System.Text.Encoding.UTF8;
            mail.Body = content;
            mail.BodyEncoding = System.Text.Encoding.UTF8;
            mail.IsBodyHtml = true;

            SmtpClient client = new();
            client.Credentials = new System.Net.NetworkCredential(options.Sender, options.Password);
            client.Host = options.Host;

            return (client, mail);
        }

        /// <summary>
        /// 发送一封邮箱，该邮件内容可以是HTML内容，发送成功后返回。
        /// </summary>
        /// <param name="receiver">邮件接收者</param>
        /// <param name="content">邮件内容</param>
        /// <param name="title">标题</param>
        /// <exception cref="InternalException"></exception>
        public void SendTo(string receiver, string content, string title)
        {
            var (client, mail) = CreateMail(receiver, content, title);
            try
            {
                client.Send(mail);
            }
            catch (SmtpException ex)
            {
                throw new InternalException("Email发送失败", ex);
            }
        }

        /// <summary>
        /// 异步发送一封邮箱，该邮件内容可以是HTML内容。
        /// </summary>
        /// <param name="receiver">邮件接收者</param>
        /// <param name="content">邮件内容</param>
        /// <param name="title">标题</param>
        /// <exception cref="InternalException"></exception>
        public Task SendToAsync(string receiver, string content, string title)
        {
            var (client, mail) = CreateMail(receiver, content, title);
            try
            {
                return client.SendMailAsync(mail);
            }
            catch (SmtpException ex)
            {
                throw new InternalException("Email发送失败", ex);
            }
        }
    }
}