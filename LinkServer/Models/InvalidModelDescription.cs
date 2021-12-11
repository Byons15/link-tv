using Microsoft.AspNetCore.Mvc;
using System.Collections.Generic;
using System.ComponentModel;
using System;
using System.Reflection;
using NJsonSchema.Generation;

namespace LinkServer.Models
{
    public class InvalidModelDescription
    {
        /// <summary>
        /// 错误类型描述帮助URL
        /// </summary>
        [ModelBinder(Name = "type")]
        [DefaultValue("")]
        public string Type { get; set; }

        /// <summary>
        /// 标题
        /// </summary>
        [ModelBinder(Name = "title")]
        [DefaultValue("")]
        public string Title { get; set; }

        /// <summary>
        /// 状态码，总是400
        /// </summary>
        [ModelBinder(Name = "status")]
        [DefaultValue(400)]
        public int Status { get; set; }

        /// <summary>
        /// 错误跟踪码
        /// </summary>
        [ModelBinder(Name = "traceId")]
        [DefaultValue("")]
        public string TraceId { get; set; }

        /// <summary>
        /// 错误参数及其描述
        /// </summary>
        [ModelBinder(Name = "errors")]
        public Dictionary<string, IList<string>> Errors { get; set; }
    }
}
