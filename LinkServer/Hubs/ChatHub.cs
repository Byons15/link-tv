using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.SignalR;
using System.Threading.Tasks;

namespace LinkServer.Hubs
{
    
    public class ChatHub :Hub
    {
        public async Task IntoLiveChat(long liveId)
        {
            System.Console.WriteLine($"{Context.ConnectionId}进入了聊天室{liveId}");
            await Groups.AddToGroupAsync(Context.ConnectionId, liveId.ToString());
        }

        public async Task LeaveLiveChat(long liveId)
        {
            System.Console.WriteLine($"{Context.ConnectionId}退出了聊天室{liveId}");
            await Groups.RemoveFromGroupAsync(Context.ConnectionId, liveId.ToString());
        }

        public Task SendToLiveChat(LiveChatMessage message)
        {
            System.Console.WriteLine($"{message.Name}有新的消息");
            //await Clients.Group(liveId.ToString()).SendAsync("liveChatReceived", message);
            return Clients.All.SendAsync("liveChatReceived", message);
        }
    }

    public class LiveChatMessage
    {
        public string Name { get; set; }
        public string Message { get; set; }
    }
}
