using Microsoft.AspNetCore.Mvc;
using System.ComponentModel;

namespace LinkServer.Components
{
    /// <summary>
    /// 用于描述服务器内部发生错误的响应模型
    /// </summary>
    public class InternalErrorDescription
    {
        /// <summary>
        /// 总是“内部服务器错误。”
        /// </summary>
        [ModelBinder(Name = "title")]
        [DefaultValue("内部服务器错误。")]
        public string Title { get; set; }

        /// <summary>
        /// 追踪ID，客户端应及时将此ID反馈回开发人员
        /// </summary>
        [ModelBinder(Name = "traceId")]
        [DefaultValue("")]
        public string TraceId { get; set; }
    }
}
