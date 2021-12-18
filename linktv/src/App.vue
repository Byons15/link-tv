<script setup lang="ts">
import { inject, onMounted, provide, reactive } from "@vue/runtime-core";
import { nextTick, ref } from "vue";
import ErrorModal from "./components/ErrorModal.vue";
import $ from "jquery";
import * as Utils from "./Utils";
import axios from "axios";
import { IInvalidModelDescription, IUserDTO, UserClient } from "./LinkClient";

const globalErrorModal = ref<any>(null);

provide("errorModal", globalErrorModal);

const userStore = reactive<Utils.IUserStore>({
  id: 0,
  name: "",
  email: "",
  token: "",
  logined: false,
  login: null,
  logout: null,
  update: null,
});

userStore.login = (token: string) => {
  axios.defaults.headers.common["Authorization"] = "Bearer " + token;
  localStorage.setItem("token", token);
  userStore.update();
  userStore.token = token;
};

userStore.logout = ()=>{
  userStore.logined = false;
  userStore.id = 0;
  userStore.email = '';
  userStore.token = '';
  userStore.name = '';

  axios.defaults.headers.common["Authorization"] = '';
  localStorage.removeItem("token");
};

userStore.update = ()=>{
  const client = new UserClient();
  client.get(0)
  .then((userDto: IUserDTO)=>{
    userStore.id = userDto.id;
    userStore.name = userDto.name;
    userStore.email = userDto.email;
    if (userDto.image !== undefined) {
      var file = userDto.image as any;
      userStore.image = "data:" + file.contentType + ";base64," + file.fileContents;
    }
    userStore.logined = true;
  })
  .catch((error: IInvalidModelDescription)=>{
    globalErrorModal.value.show(error.title);
    userStore.logout();
  })
  .catch((error)=>{
    console.log("发生未预料的错误。");
    console.log(error);
    globalErrorModal.value.show();
    userStore.logout();
  });
};

provide("userStore", userStore);

onMounted(() => {
  nextTick(() => {
    let token = localStorage.getItem("token");
    if(token != null){
      console.log("检索到本地token, 准备尝试使用其登录");
      userStore.login(token);
    }
  });
});
</script>

<template>
  <div>
    <router-view></router-view>
    <Teleport to="body">
      <ErrorModal ref="globalErrorModal"></ErrorModal>
    </Teleport>
  </div>
</template>
<style>
</style>
