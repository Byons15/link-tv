using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.ComponentModel.DataAnnotations;

namespace LinkServer.Models
{
    /// <summary>
    /// 用户信息
    /// </summary>
    public class UserDTO
    {
        /// <summary>
        /// ID
        /// </summary>
        public long Id { get; set; }

        /// <summary>
        /// 用户邮箱
        /// </summary>
        public string Email { get; set; }

        /// <summary>
        /// 用户名
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// 用户头像，用于传输时为base64编码
        /// </summary>
        public string Image { get; set; }

        /// <summary>
        /// 用户最后一次直播的时间
        /// </summary>
        public DateTime? LastLive { get; set; }

        /// <summary>
        /// 直播间名称
        /// </summary>
        public string TVName { get; set; }

        /// <summary>
        /// 直播间封面
        /// </summary>
        public string TVImage { get; set; }

        /// <summary>
        /// 默认构造
        /// </summary>
        public UserDTO()
        {

        }

        /// <summary>
        /// 使用User模型构造
        /// </summary>
        /// <param name="user"></param>
        public UserDTO(User user)
        {
            Id = user.Id;
            Email = user.Email;
            Name = user.Name;
            Image = user.Image;
            LastLive = user.LastLive;
            TVImage = user.TVImage;
            TVName = user.TVName;
        }
    }
}
