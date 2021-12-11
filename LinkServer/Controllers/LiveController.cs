using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using LinkServer.Services;
using LinkServer.Models;
using Microsoft.AspNetCore.Cors;
using Microsoft.AspNetCore.Authorization;
using LinkServer.Components;
using Microsoft.AspNetCore.Mvc.ModelBinding;
using System.Text;
using Microsoft.AspNetCore.Http;

// For more information on enabling Web API for empty projects, visit https://go.microsoft.com/fwlink/?LinkID=397860

namespace LinkServer.Controllers
{

    /// <summary>
    /// 用于直播相关的控制器
    /// </summary>
    [Route("api/[controller]")]
    [ApiController]
    [EnableCors("Any")]
    public class LiveController : ControllerBase
    {
        private readonly LiveService liveService;

        /// <summary>
        /// 提供<see cref="LiveService"/>以构造控制器
        /// 只由DI容器管理
        /// </summary>
        /// <param name="liveService"></param>
        public LiveController(LiveService liveService)
        {
            this.liveService = liveService;
        }

        /// <summary>
        /// 获取当前所有的直播
        /// </summary>
        /// <returns>所有直播间的列表，可能为空</returns>
        [HttpGet]
        public IEnumerable<Studio> All()
        {
            return liveService.All();
        }

        /// <summary>
        /// 获取指定用户的直播信息
        /// </summary>
        /// <param name="name">用户的Id或名称</param>
        /// <returns>用户信息，如果没有找到，则返回401</returns>
        [HttpGet("{name}")]
        public Studio Get(string name)
        {
            return liveService.Find(name);
        }

        /// <summary>
        /// 用于Nginx通知服务有新的推流
        /// </summary>
        /// <param name="model">Nginx传来的各种参数</param>
        [HttpPost("nginx/push")]
        public void NginxRtmpPush([FromForm]NginxStream model)
        {
            liveService.Push(new Stream(model));
            Console.WriteLine($"{model.StreamId}开始推流");
        }

        /// <summary>
        /// 用户Nginx通知服务流已断开
        /// </summary>
        /// <param name="model"></param>
        [HttpPost("nginx/push_done")]
        public void NginxRtmpPushDone([FromForm] NginxStream model)
        {
            liveService.PushDone(new Stream(model));
            Console.WriteLine($"{model.StreamId}结束推流");
        }

        [HttpPost("tencentStreamPush")]
        public void TencentStreamPush([FromBody] TencentLiveStream model)
        {
            Stream stream = new ()
            {
                StreamId = model.stream_id,
            };

            var queryStrings = model.stream_param.Split('&');
            foreach(var q in queryStrings)
            {
                var pair = q.Split('=');
                pair[0] = pair[1].Trim();

                if (pair[0] == "txSecret")
                    stream.TxSecret = pair[1].Trim();

                if (pair[0] == "txTime")
                    stream.TxTime = pair[1].Trim();
            }

            liveService.Push(stream);
        }

        [HttpPost("tencentStreamPushDone")]
        public void TencentStreamPushDone([FromBody] TencentLiveStream model)
        {
            Stream stream = new ()
            {
                StreamId = model.stream_id,
            };

            var queryStrings = model.stream_param.Split('&');
            foreach (var q in queryStrings)
            {
                var pair = q.Split('=');
                pair[0] = pair[1].Trim();

                if (pair[0] == "txSecret")
                    stream.TxSecret = pair[1].Trim();

                if (pair[0] == "txTime")
                    stream.TxTime = pair[1].Trim();
            }

            liveService.PushDone(stream);
        }

        /// <summary>
        /// 获取推流地址
        /// </summary>
        /// <param name="id">要推流的用户</param>
        /// <returns></returns>
        [HttpGet("address/push")]
        [Authorize]
        [UserAction]
        public string PushAddress(long id)
        {
            Console.WriteLine("push");
            return liveService.PushAddress(id);
        }
    }
}
