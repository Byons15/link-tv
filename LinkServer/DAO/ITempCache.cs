using Microsoft.Extensions.Caching.Distributed;

namespace LinkServer.DAO
{
    public interface ITempCache
    {
        IDistributedCache DistributedCache { get; }
    }
}
