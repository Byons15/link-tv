using Microsoft.EntityFrameworkCore;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace LinkServer.Models
{
    /// <summary>
    /// 用于描述直播间信息的模型
    /// </summary>
    [Table("Studios")]
    [Index(nameof(Id), nameof(Director), IsUnique = true)]
    [Index(nameof(Name), IsUnique = false)]
    public class Studio
    {
        /// <summary>
        /// 直播间ID，同时也是UP的ID
        /// </summary>
        [Key]
        [Column("id")]
        public long Id { get; set; }

        /// <summary>
        /// UP主
        /// </summary>
        [Column("director")]
        public string Director { get; set; }

        /// <summary>
        /// 名称
        /// </summary>
        [Column("name")]
        public string Name { get; set; }

        /// <summary>
        /// Flv播放地址
        /// </summary>
        [NotMapped]
        public string FlvAddress { get; set; }

        /// <summary>
        /// Hls播放地址
        /// </summary>
        [NotMapped]
        public string HlsAddress { get; set; }
    }
}
