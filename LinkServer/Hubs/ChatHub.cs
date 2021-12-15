using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.SignalR;
using System.Threading.Tasks;

namespace LinkServer.Hubs
{
    
    public class ChatHub :Hub
    {
        public async Task IntoLiveChat(long liveId)
        {
            await Groups.AddToGroupAsync(Context.ConnectionId, liveId.ToString());
        }

        public async Task LeaveLiveChat(long liveId)
        {
            await Groups.RemoveFromGroupAsync(Context.ConnectionId, liveId.ToString());
        }

        [Authorize]
        public async Task SendToLiveChat(long liveId, LiveChatMessage message)
        {
            await Clients.Group(liveId.ToString()).SendAsync("liveChatReceived", message);
        }
    }

    public class LiveChatMessage
    {
        public string Name { get; set; }
        public string Message { get; set; }
    }
}
