using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Threading.Tasks;

namespace LinkServer.Models
{
    /// <summary>
    /// 用户模型
    /// </summary>
    [Table("Users")]
    [Index(nameof(Name), nameof(Id), nameof(Email), IsUnique = true)]
    public class User
    {
        /// <summary>
        /// 用户的唯一Id
        /// </summary>
        [Key]
        [Column("id", TypeName = "BIGINT")]
        public long Id { get; set; }

        /// <summary>
        /// 用户密码
        /// </summary>
        [Column("password", TypeName ="CHAR(128)")]
        public string Password { get; set; }

        /// <summary>
        /// 用户的唯一邮箱
        /// </summary>
        [EmailAddress]
        [Column("email", TypeName = "CHAR(64)")]
        public string Email { get; set; }

        /// <summary>
        /// 用户的唯一名
        /// </summary>
        [Column("name", TypeName = "CHAR(64)")]
        public string Name { get; set; }

        /// <summary>
        /// 用户头像的位置
        /// </summary>
        [Column("image", TypeName = "VARCHAR(256)")]
        public string Image { get; set; }

        /// <summary>
        /// 用户直播间的名称
        /// </summary>
        [Column("tv_name")]
        public string TVName { get; set; }

        /// <summary>
        /// 直播间封面位置
        /// </summary>
        [Column("tv_image")]
        public string TVImage { get; set; }

        /// <summary>
        /// 最后一次直播的时间
        /// </summary>
        [Column("last_live")]
        public DateTime? LastLive { get; set; }
    }
}
