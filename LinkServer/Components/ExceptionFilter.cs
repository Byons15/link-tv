
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;
using System.Threading.Tasks;

namespace LinkServer.Components
{
    /// <summary>
    /// 对控制器内的所有异常进行过滤转换为对用户的响应
    /// </summary>
    public class ExceptionFilter : IExceptionFilter
    {
        public void OnException(ExceptionContext context)
        {
            BusinessExceptionHandle(context);
        }

        private void BusinessExceptionHandle(ExceptionContext context)
        {
            if (context.Exception is BusinessException)
            {
                var ex = context.Exception as BusinessException;
                context.Result = new ContentResult()
                {
                    StatusCode = ex.StatusCode,
                    Content = ex.Message,
                    ContentType = ex.MessageType
                };
            }
            else if(context.Exception is not null)
            {
                context.Result = new JsonResult(new InternalErrorDescription())
                {
                    StatusCode = 500
                };
            }

            context.ExceptionHandled = true;
        }
    }
}
