<script setup lang="ts">
import * as SignalR from "@microsoft/signalr";
import { inject, nextTick, onMounted, ref } from "vue";
import * as Utils from "../Utils";
import {IUserDTO, UserClient, UserDTO} from "../LinkClient";

let connection: SignalR.HubConnection;
const userName = ref("");
let userId = 0;

onMounted(() => {
  nextTick(() => {
    connection = new SignalR.HubConnectionBuilder()
    .withUrl("http://localhost:5000/chatHub", { accessTokenFactory: () =>{
        return localStorage.getItem("token");
    } })
    .build();

    connection.start().catch((error)=>{
        Utils.errorModal.value.show(error);
    }).then(()=>{

      const client = new UserClient();
      client.get(0).then((user: IUserDTO)=>{
        userName.value = user.name;
        userId = user.id;
      }).catch(()=>{
        Utils.errorModal.value.show();
      });

      connection.send("IntoLiveChat", userId).catch(()=>{
        Utils.errorModal.value.show();
      }).then(()=>{
        console.log("成功进入聊天室");
      });
    });
  });
});

function onSend(){
    connection.send("SendToLiveChat", )
}

</script>

<template>
  <div class="d-flex flex-column px-0 chatHub-container">
    <div
      class="flex-grow-1 bg-light"
      style="overflow-y: auto; overflow-x: hidden"
    ></div>
    <div class="input-group">
      <input type="text" class="form-control" />
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