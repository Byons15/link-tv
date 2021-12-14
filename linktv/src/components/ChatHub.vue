<script setup lang="ts">
import { ref } from "@vue/reactivity";
import * as signalR from "@microsoft/signalr";
import ErrorModal from "./ErrorModal.vue";
import { nextTick } from "@vue/runtime-core";

class Message {
  username?: string;
  message?: string;
}

const tempMsg = <Message>{
  message: "teshi",
  username: "dfaef",
};

const chatContainer = ref<HTMLDivElement>();

const messages = ref<Message[]>(new Array<Message>());

const connection = new signalR.HubConnectionBuilder()
  .withUrl("http://localhost:5000/chatHub")
  .build();

const inputMessage = ref("");
connection.on("messageReceived", (username: string, message: string) => {
  messages.value.push(<Message>{
    message: message,
    username: username,
  });

    nextTick(()=>{
        chatContainer.value.scrollTop = chatContainer.value.scrollHeight;
    });

});

const hubError = ref("");

connection.start().catch((error: string) => {
  hubError.value = error;
});

function onSendMsg() {
  if (inputMessage.value.length == 0) return;

  connection.send("newMessage", 4544, inputMessage.value).then(() => {
    inputMessage.value = "";
  });
}
</script>

<template>
  <div class="d-flex flex-column px-0">
    <div class="flex-grow-1 px-2 chat-container overflow-auto" ref="chatContainer">
      <p
        v-for="(msg, i) in messages"
        :key="i"
        class="my-1"
        style="word-wrap: break-word; word-break: break-all; overflow: hidden"
      >
        {{ msg.username }}: {{ msg.message }}
      </p>
    </div>
    <div class="input-group">
      <input
        class="form-control"
        v-model="inputMessage"
        @keypress.enter="onSendMsg"
      />
      <span class="input-group-append">
        <button class="btn btn-primary btn-sm" @click="onSendMsg">发送</button>
      </span>
    </div>
    <ErrorModal v-if="hubError != ''" :msg="hubError"></ErrorModal>
  </div>
</template>

<style scoped>
.chat-container {
  height: 456px;
}

@media (min-width: 992px) {
  .chat-container {
    height: 456px;
  }
}

@media (min-width: 768px) {
  .chat-container {
    height: 350px;
  }
}

@media (min-width: 1200px) {
  .chat-container {
    height: 530px;
  }
}
</style>