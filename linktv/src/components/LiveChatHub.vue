<script setup lang="ts">
import $ from "jquery";
import * as SignalR from "@microsoft/signalr";
import { inject, nextTick, onMounted, ref } from "vue";
import * as Utils from "../Utils";
import { watch } from "vue";

let connection: SignalR.HubConnection;

const errorModal = inject<any>("errorModal");
const userStore = inject<Utils.IUserStore>("userStore");

const message = ref("");

const props = defineProps({
  liveName: String,
});

class LiveChatMessage {
  liveName?: string;
  name?: string;
  message?: string;
}

const messageList = ref<LiveChatMessage[]>(new Array<LiveChatMessage>());

function connectToServer() {
  if (
    props.liveName === undefined ||
    props.liveName.length == 0 ||
    props.liveName == "0"
  )
    return;

  console.log("正在进入", props.liveName, "的直播间");
  connection = new SignalR.HubConnectionBuilder()
    .withUrl("http://localhost:5000/LiveChatHub", {
      accessTokenFactory: () => userStore.token,
    })
    .configureLogging(SignalR.LogLevel.Debug)
    .build();

  connection.on("liveChatReceived", (msg) => {
    console.log(msg);
  });

  connection
    .start()
    .catch((error) => {
      errorModal.value.show();
    })
    .then(() => {
      console.log("聊天hub连接成功");
      connection.send("IntoLiveChat", props.liveName).then(() => {
        console.log("成功进入直播间聊天室");
      });
    });
}

watch(
  () => props.liveName,
  () => {
    connectToServer();
  }
);

onMounted(() => {
  nextTick(() => {
    connectToServer();
  });
});

function onSend() {
  if (message.value.length == 0) return;

  connection
    .send("SendToLiveChat", <LiveChatMessage>{
      liveName: props.liveName,
      name: userStore.name,
      message: message.value,
    })
    .catch((error) => {
      console.log(error);
      errorModal.value.show(error);
    })
    .then(() => {
      console.log("发送完成");
    })
    .finally(() => {
      message.value = "";
    });
}
</script>

<template>
  <div class="d-flex flex-column px-0 chatHub-container">
    <div
      class="flex-grow-1 bg-light"
      style="overflow-y: auto; overflow-x: hidden"
    >
    <p v-for="(msg, i) in messageList" :key="i">{{msg.name}}: {{msg.message}}</p>
    </div>
    <div class="input-group">
      <input type="text" class="form-control" v-model="message" />
      <span class="input-group-append">
        <button class="btn btn-primary btn-sm" @click="onSend">发送</button>
      </span>
    </div>
  </div>
</template>

<style scoped>
.chatHub-container {
  height: 293px;
}
</style>