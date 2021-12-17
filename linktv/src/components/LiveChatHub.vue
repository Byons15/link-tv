<script setup lang="ts">
import $ from 'jquery';
import * as SignalR from "@microsoft/signalr";
import { inject, nextTick, onMounted, ref } from "vue";
import * as Utils from "../Utils";
import Router from "../router";

let connection: SignalR.HubConnection;
const userName = ref("");
let userId = 0;

const errorModal = inject<any>("errorModal");
const userStore = inject<Utils.IUserStore>("userStore");

const message = ref("");
let liveId = 0;

onMounted(() => {
  nextTick(() => {
    liveId = Number(Router.currentRoute.value.params.id as string);
    connection = new SignalR.HubConnectionBuilder().withUrl("http://localhost:5000/ChatHub", {accessTokenFactory: ()=>
         userStore.token
      }).build();

    connection.on("liveChatReceived", (msg)=>{
      console.log(msg);
    });

    connection.start().catch((error) => {
        errorModal.value.show();
      }).then(()=>{
        console.log("聊天hub连接成功");
      });
  });
});

function onSend() {
  if (message.value.length == 0) return;

  connection
    .send("SendToLiveChat", {
      Name: userStore.name,
      Message: message.value,
    })
    .catch((error) => {
      console.log(error);
      errorModal.value.show(error);
    })
    .then(()=>{
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
    ></div>
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