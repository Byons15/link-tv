using LinkServer.Models;
using System;
using LinkServer.Components;

namespace LinkServer.Services
{
    /// <summary>
    /// 用于实现与流服务器交互的接口
    /// </summary>
    public interface ILiveServer
    {

        /// <summary>
        /// 接口实现断开指定流
        /// </summary>
        /// <param name="streamId"></param>
        void DropStream(string streamId);

        /// <summary>
        /// 接口实现响应鉴权码更改
        /// </summary>
        /// <param name="masterKey"></param>
        /// <param name="backupKey"></param>
        void OnAuthorizationKeyChanged(string masterKey, string backupKey);

        /// <summary>
        /// 接口实现根据流Id返回Hls协议的播放地址
        /// </summary>
        /// <param name="streamId">流</param>
        /// <returns>该流的Hls播放地址</returns>
        string HLSPlayAddress(string streamId);

        /// <summary>
        /// 接口实现根据流Id返回Http flv协议的播放地址
        /// </summary>
        /// <param name="streamId">流</param>
        /// <returns>该流的Flv播放地址</returns>
        string FLVPlayAddress(string streamId);

        /// <summary>
        /// 接口实现返回流服务器的地址，包括端口 例如 192.168.1.1:1935
        /// </summary>
        string Host { get; }

        /// <summary>
        /// 接口实现返回流服务器的应用名。
        /// </summary>
        string AppName { get; }
    }
}
