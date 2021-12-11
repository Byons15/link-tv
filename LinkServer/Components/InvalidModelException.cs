using Newtonsoft.Json;
using System;
using LinkServer.Models;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Configuration;
using Microsoft.AspNetCore.Mvc;
using System.Linq;
using System.Collections.Generic;

namespace LinkServer.Components
{

    /// <summary>
    /// 无效模型异常
    /// </summary>
    [Serializable]
    public class InvalidModelException : BusinessException
    {
        private readonly InvalidModelDescription responseModel = new();

        public override int StatusCode { get => 400; }

        private void AppendParamErrorDesc(string param, string desc)
        {
            responseModel.Errors ??= new Dictionary<string, IList<string>>();
            if (!responseModel.Errors.TryGetValue(param, out _))
            {
                IList<string> paramDescs = new List<string> { desc };
                responseModel.Errors.Add(param, paramDescs);
            }
            else
            {
                _ = responseModel.Errors[param].Append(desc);
            }
        }

        /// <summary>
        /// 使用语义化消息抛出异常
        /// </summary>
        /// <param name="msg"></param>
        public InvalidModelException(string msg)
        {
            responseModel.Title = msg;
        }

        /// <summary>
        /// 使用语义化消息和无效参数名抛出异常
        /// </summary>
        /// <param name="msg"></param>
        /// <param name="param"></param>
        public InvalidModelException(string msg, string param)
        {
            responseModel.Title = "One parameter is invalid";
            AppendParamErrorDesc(param, msg);
        }

        /// <summary>
        /// 使用语义化标题和多个无效参数名和错误描述抛出异常
        /// </summary>
        /// <param name="title"></param>
        /// <param name="invalidParams"></param>
        public InvalidModelException(string title, Dictionary<string, string[]> invalidParams = null)
        {
            responseModel.Title = title;
            responseModel.Errors = new Dictionary<string, IList<string>>();
            foreach (var param in invalidParams)
            {
                responseModel.Errors[param.Key] = param.Value;
            }
        }

        /// <summary>
        /// 使用语义化消息和无效参数名抛出异常，同时附带内部异常
        /// </summary>
        /// <param name="msg"></param>
        /// <param name="inner"></param>
        /// <param name="param"></param>
        public InvalidModelException(string msg, Exception inner, string param)
            : base(inner)
        {
            responseModel.Title = "One parameter is invalid";
            AppendParamErrorDesc(param, msg);
        }

        /// <summary>
        /// 使用语义化标题和多个无效参数名和错误描述抛出异常，同时附带内部异常
        /// </summary>
        /// <param name="title"></param>
        /// <param name="inner"></param>
        /// <param name="invalidParams"></param>
        public InvalidModelException(string title, Exception inner, Dictionary<string, string[]> invalidParams = null)
            :base(inner)
        {
            responseModel.Title = title;
            responseModel.Errors = new Dictionary<string, IList<string>>();
            foreach (var param in invalidParams)
            {
                responseModel.Errors[param.Key] = param.Value;
            }
        }

        protected InvalidModelException(
          System.Runtime.Serialization.SerializationInfo info,
          System.Runtime.Serialization.StreamingContext context) : base(info, context) { }

        public override string Message 
        { 
            get
            {
                return System.Text.Json.JsonSerializer.Serialize(responseModel, 
                    typeof(InvalidModelDescription), 
                    new System.Text.Json.JsonSerializerOptions(System.Text.Json.JsonSerializerDefaults.Web));
            }
        }

        public override string MessageType { get => "application/json;charset=UTF-8"; }
    }

    /// <summary>
    /// 扩展配置当框架默认模型验证失败时，自定义响应格式
    /// </summary>
    public static class InvalidModelResponseExtensions
    {
        public static IServiceCollection AddInvalidModelResponseExtensions(
             this IServiceCollection services)
        {
            services.Configure<ApiBehaviorOptions>((options) =>
            {
                options.InvalidModelStateResponseFactory = actionContext =>
                {
                    //获取验证失败的模型字段 
                    var errors = from value in actionContext.ModelState.Values
                                 where value.Errors.Count > 0
                                 select value;

                    var errorList = errors.ToList();

                    //设置返回内容
                    throw new NotImplementedException();
                };
            });

            return services;
        }
    }
}
