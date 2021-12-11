using LinkServer.Services;
using System.ComponentModel.DataAnnotations;

namespace LinkServer.Models
{
    /// <summary>
    /// <see cref="LiveService"/>所用流模型
    /// </summary>
    public class Stream
    {
        /// <summary>
        /// 流携带的验证字串
        /// </summary>
        [Required]
        public string TxSecret { get; set; }

        /// <summary>
        /// 流携带的超时字串
        /// </summary>
        [Required]
        public string TxTime { get; set; }

        /// <summary>
        /// 流ID
        /// </summary>
        [Required]
        public string StreamId { get; set; }


        /// <summary>
        /// 默认构造
        /// </summary>
        public Stream()
        {

        }

        /// <summary>
        /// 使用NginxStream模型构造
        /// </summary>
        /// <param name="model"></param>
        public Stream(NginxStream model)
        {
            TxSecret = model.Secret;
            TxTime = model.ExpireTime;
            StreamId = model.StreamId;
        }
    }
}
