using CSRedis;
using Microsoft.Extensions.Caching.Distributed;
using Microsoft.Extensions.Caching.Redis;

namespace LinkServer.DAO
{
    public class TempCache : CSRedisClient, ITempCache
    {
        private readonly CSRedisCache redisCache;

        public TempCache(string connectionString)
            : base(connectionString)
        {
            redisCache = new CSRedisCache(this);
        }

        public IDistributedCache DistributedCache => redisCache;
    }
}
