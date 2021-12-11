using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LinkServer.Components
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Method)]
    public class RobotLimitAttribute : Attribute, IResourceFilter
    {
        public void OnResourceExecuted(ResourceExecutedContext context)
        {
            // pass
        }

        public void OnResourceExecuting(ResourceExecutingContext context)
        {
            // TODO:: 使用一点策略来限制客户端的频繁请求，避免被脚本攻击
        }
    }
}
