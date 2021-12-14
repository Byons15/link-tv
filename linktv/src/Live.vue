<script setup lang="ts">
import { ref } from "@vue/reactivity";
import Player from "./components/LivePlayer.vue";
import Telecasts from "./components/Telecasts.vue";
import Navbar from "./components/navbar.vue";
import { getCurrentInstance, nextTick, onMounted } from "@vue/runtime-core";
import Router from "./Router";
import { LiveClient, IStudio, IInvalidModelDescription, Studio } from "./LinkClient";
import ErrorModal from "./components/ErrorModal.vue";
import ChatHub from "./components/ChatHub.vue";

const errorMsg = ref("");

const playerWidescreen = ref(false);

const tvName = ref<string>();
const director = ref<string>();
let directorId: number = 0;

const liveUrl = "http://byons.tpddns.cn:62407/linktv?app=linktv&stream=";
const videoSrc = ref(liveUrl + directorId);

interface Address {
  address?: string;
}
onMounted(() => {
  nextTick(() => {
    console.log("live player on mounted");
    // const liveApi = new LiveApi();
    // liveApi
    //   .apiLiveNameGet(Router.currentRoute.value.params.id as string)
    //   .then((response) => {
    //     if (response.status == 200) {
    //       tvName.value = response.data.name;
    //       director.value = response.data.director;
    //       directorId = response.data.id;
    //       videoSrc.value = response.data.flvAddress;
    //     }
    //   });
    const api = new LiveClient();
    api.get(Router.currentRoute.value.params.id as string)
    .catch((error : IInvalidModelDescription)=>{
      errorMsg.value = error.errors.name !== undefined ? error.errors.name[0] : '未知错误';
    })
    .catch(()=>{
      errorMsg.value = '不知道什么问题';
    })
    .then((studio: Studio)=>{
      directorId = studio.id;
      director.value = studio.director;
      tvName.value = studio.name;
      videoSrc.value = studio.flvAddress;
    })
  });
});
</script>

<template>
  <div>
    <Navbar></Navbar>
    <div class="container">
      <div class="row">
        <Player
          class="col-12"
          :title="tvName"
          :anchor="director"
          :video_src="videoSrc"
          @switchWidescreen="playerWidescreen = $event"
          :class="{ 'col-md-9': !playerWidescreen }"
          ref="playerRef"
        ></Player>
        <ChatHub :class="{ 'col-md-3': !playerWidescreen }" v-if="!playerWidescreen"></ChatHub>
      </div>
    </div>
    <Teleport to='body' v-if="errorMsg !== ''">
      <ErrorModal :msg = "errorMsg"></ErrorModal>
    </Teleport>
  </div>
</template>