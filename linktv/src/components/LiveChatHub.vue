<script setup lang="ts">
import * as SignalR from "@microsoft/signalr";
import { inject, nextTick, onMounted, ref } from "vue";
import ErrorModal from "./ErrorModal.vue";

let connection: SignalR.HubConnection;

const haveError = ref("");
const errorModal = ref("");

const globalUtils = inject<any>("globalUtils");

onMounted(() => {
  nextTick(() => {
    connection = new SignalR.HubConnectionBuilder()
    .withUrl("http://localhost:5000/chatHub", { accessTokenFactory: () =>{
        return localStorage.getItem("token");
    } })
    .build();

    connection.start().catch((error)=>{
        globalUtils.showError(error);
    });
  });
});

function onSend(){
    globalUtils.showError("fhose");
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
    <ErrorModal v-if="haveError != ''" :msg="haveError" ref="errorModal"></ErrorModal>
  </div>
</template>

<style scoped>
.chatHub-container {
  height: 293px;
}
</style>