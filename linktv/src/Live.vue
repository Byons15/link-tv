<script setup lang="ts">
import { ref } from "@vue/reactivity";
import Player from "./components/LivePlayer.vue";
import Navbar from "./components/navbar.vue";
import { inject, nextTick, onMounted } from "@vue/runtime-core";
import Router from "./Router";
import {
  LiveClient,
  IStudio,
  IInvalidModelDescription,
  Studio,
} from "./LinkClient";
import ChatHub from "./components/LiveChatHub.vue";

const playerWidescreen = ref(false);

const errorModal = inject<any>("errorModal");

const tvName = ref<string>();
const director = ref<string>();
const liveName = ref("");
let directorId = 0;

const liveUrl = "";
const videoSrc = ref("");

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

    console.log(Router.currentRoute.value.params.id as string);

    const api = new LiveClient();
    api
      .get(Router.currentRoute.value.params.id as string)
      .catch((error: IInvalidModelDescription) => {
        errorModal.value.show(
          error.errors.name !== undefined ? error.errors.name[0] : "未知错误"
        );
      })
      .catch(() => {
        errorModal.value.show();
      })
      .then((studio: Studio) => {
        directorId = studio.id;
        director.value = studio.director;
        tvName.value = studio.name;
        videoSrc.value = studio.flvAddress;

        liveName.value = directorId.toString();
      });
  });
});
</script>

<template>
  <div>
    <Navbar></Navbar>
    <div class="container">
      <div class="row w-100 h-100">
        <Player
          class="col-12"
          :title="tvName"
          :anchor="director"
          :video_src="videoSrc"
          @switchWidescreen="playerWidescreen = $event"
          :class="{ 'col-md-9': !playerWidescreen }"
          ref="playerRef"
        ></Player>
        <ChatHub
          class="chat-hub d-none col-md-3"
          v-if="!playerWidescreen"
          :live-name="liveName"
        ></ChatHub>
      </div>
    </div>
  </div>
</template>

<style scoped>

.chat-hub {
      opacity: 1;
}

@media screen and (max-width: 576px) {
  .chat-hub {
    opacity: 0;
  }
}

@media screen and (min-width: 576px)and (max-width: 768px) {
  .chat-hub {
    opacity: 0;
  }
}

@media screen and (min-width: 768px) and ( max-width:992px ) {
  .chat-hub {
    height: 344px;
  }
}

@media screen and (min-width: 992px) and (max-width: 1200px){
  .chat-hub {
    height: 445px;
  }
}

@media screen and (min-width: 1200px) {
  .chat-hub {
   height: 522px;
  }
}

</style>