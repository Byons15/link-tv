using Microsoft.AspNetCore.Mvc.Filters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LinkServer.Components
{
    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Method)]
    public class UserActionAttribute : Attribute, IActionFilter
    {
        public void OnActionExecuted(ActionExecutedContext context)
        {
            // pass
        }

        public void OnActionExecuting(ActionExecutingContext context)
        {
            context.ActionArguments["id"] =
                long.Parse(context.HttpContext.User.Claims
                            .Single(new Func<System.Security.Claims.Claim, bool>(claim => claim.Type == "id"))
                            .Value);
        }
    }
}
