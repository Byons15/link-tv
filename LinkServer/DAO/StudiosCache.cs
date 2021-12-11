using CSRedis;
using Microsoft.Extensions.Configuration;

namespace LinkServer.DAO
{
    public class StudiosCache :CSRedisClient
    {
        public StudiosCache(IConfiguration configuration)
            : base(configuration.GetConnectionString("StudiosCacheRedisConnection"))
        {

        }
    }
}
