using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;
using System.Threading.Tasks;

namespace LinkServer.Hubs
{
    
    public class ChatHub :Hub
    {
        public async Task IntoLiveChat(string liveName)
        {
            System.Console.WriteLine($"{Context.ConnectionId}进入了聊天室{liveName}");
            await Groups.AddToGroupAsync(Context.ConnectionId, liveName);
        }

        public async Task LeaveLiveChat(string liveName)
        {
            System.Console.WriteLine($"{Context.ConnectionId}退出了聊天室{liveName}");
            await Groups.RemoveFromGroupAsync(Context.ConnectionId, liveName);
        }

        public async Task SendToLiveChat(LiveChatMessage message)
        {
            System.Console.WriteLine($"{message.Name}有新的消息");
            await Clients.Group(message.LiveName).SendAsync("liveChatReceived", message);
        }
    }

    public class LiveChatMessage
    {
        public string LiveName { get; set; }
        public string Name { get; set; }
        public string Message { get; set; }
    }
}
