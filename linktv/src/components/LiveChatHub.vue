<script setup lang="ts">
import $ from "jquery";
import * as SignalR from "@microsoft/signalr";
import { inject, nextTick, onMounted, ref } from "vue";
import * as Utils from "../Utils";
import { watch } from "vue";
import { getHashes, Hash, randomBytes } from "crypto";
import { Md5 } from "ts-md5/dist/md5";

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
const chatContainer = ref<HTMLDivElement>();

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

  connection.on("liveChatReceived", (msg: LiveChatMessage) => {
    messageList.value.push(msg);
    nextTick(() => {
      if (
        chatContainer.value.scrollHeight !== undefined &&
        chatContainer.value.scrollHeight != null
      )
        chatContainer.value.scrollTop = chatContainer.value.scrollHeight;
    });
  });

  connection
    .start()
    .catch((error) => {
      errorModal.value.show();
    })
    .then(() => {
      console.log("聊天hub连接成功");
      connection
        .send("IntoLiveChat", {
          liveName: props.liveName,
          name: userStore.logined
            ? userStore.name
            : "游客" + Md5.hashStr(connection.connectionId).substring(0, 8),
          message: "",
        })
        .then(() => {
          console.log("成功进入直播间聊天室");
        });
    });
}

watch(
  () => props.liveName,
  () => {
    if (connection != null)
      connection.stop().then(() => {
        connectToServer();
      });
    else connectToServer();
  }
);

watch(
  () => userStore.logined,
  () => {
    if (connection != null)
      connection.stop().then(() => {
        connectToServer();
      });
    else connectToServer();
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
    .then(() => {})
    .finally(() => {
      message.value = "";
    });
}
</script>

<template>
  <div class="d-flex flex-column px-0 chatHub-container">
    <div
      class="flex-grow-1 bg-light"
      style="overflow-y: auto"
      ref="chatContainer"
    >
      <p v-for="(msg, i) in messageList" :key="i" class="px-1 my-1 small">
        {{ msg.name }}: {{ msg.message }}
      </p>
      <br />
    </div>
    <div class="input-group">
      <input
        type="text"
        class="form-control"
        v-model="message"
        @keypress.enter="onSend"
        :disabled="!(userStore.logined)"
        :placeholder="userStore.logined ? '' : '请先登录'"
      />
      <span class="input-group-append">
        <button class="btn btn-primary btn-sm" @click="onSend" :disabled="!(userStore.logined)">发送</button>
      </span>
    </div>
  </div>
</template>

<style scoped>
p {
  word-wrap: break-word;
  word-break: break-all;
  overflow: hidden;
}
</style>