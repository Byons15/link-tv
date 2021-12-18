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
        public async Task IntoLiveChat(string liveName)
        {
            await Groups.AddToGroupAsync(Context.ConnectionId, liveName);

            //if (!Context.Items.TryGetValue("groups", out _))
            //    Context.Items.Add("groups", new List<string>());

            //(Context.Items["groups"] as List<string>).Add(liveName);
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
