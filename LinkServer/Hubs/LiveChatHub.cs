using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.SignalR;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace LinkServer.Hubs
{
    public class LiveChatHub :Hub
    {
        public async Task IntoLiveChat(LiveChatMessage model)
        {
            await Groups.AddToGroupAsync(Context.ConnectionId, model.LiveName);

            await Clients.Group(model.LiveName).SendAsync("liveChatReceived", new LiveChatMessage()
            {
                LiveName = model.LiveName,
                Name = "系统消息",
                Message = $"{model.Name}进入了直播间。"
            });
        }

        public async Task LeaveLiveChat(string liveName)
        {
            await Groups.RemoveFromGroupAsync(Context.ConnectionId, liveName);
        }

        [Authorize]
        public async Task SendToLiveChat(LiveChatMessage message)
        {
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
