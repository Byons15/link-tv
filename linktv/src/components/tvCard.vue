<script setup lang="ts">
import { ref } from "@vue/reactivity";
import FlvPlayer from "./FlvPlayer.vue";
import Router from "../Router";
import { nextTick, onMounted } from "@vue/runtime-core";
import LiveGuideVue from "./LiveGuide.vue";
import { LiveClient } from "../LinkClient";

const props = defineProps<{
  liveId: number;
  playAddress: string;
  title: string;
  director: string;
}>();

const playing = ref<boolean>(true);
const volume = ref<number>(0);

interface Address{
  address?: string
}

onMounted(() => {
  nextTick(() => {
    // const api = new LiveApi();
    // api.apiLiveNameGet(props.liveId.toString()).then((response) => {
    //   if(response.status !== 200) return;
    //   director.value = response.data.director;
    //   title.value = response.data.name;
    //   playAddress.value = response.data.flvAddress;
    // });
  });
});

function onMouseover() {
  playing.value = true;
}

function onMouseleave() {
  playing.value = false;
}

function toLive() {
  Router.push("/live/" + props.liveId.toString());
}
</script>

<template>
  <div class="card tv-card p-0 border-0" @click="toLive">
    <div class="overflow-hidden">
      <div
        class="min-player"
        @mouseover="onMouseover"
        @mouseleave="onMouseleave"
      >
        <FlvPlayer
          :video_src="playAddress"
          :playing="playing"
          :volume="volume"
        ></FlvPlayer>
      </div>
    </div>
    <div class="card-body px-2 py-0">
      <div class="card-title mb-1">{{ title }}</div>
      <div class="d-flex justify-content-between">
        <div class="small director">
          {{ director }}
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.tv-card {
  cursor: pointer;
}

.director:hover {
  color: #2196f3;
}

.min-player {
  width: 100%;
  height: 100%;
  border: 1px solid transparent;
  border-radius: 12px;
  transition: 0.1s all ease-in-out;
  overflow: hidden;
}

.min-player:hover {
  transform: scale(1.05);
}
</style>