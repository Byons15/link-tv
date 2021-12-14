<script setup lang="ts">
import { ref } from "@vue/reactivity";
import { nextTick, onBeforeUnmount, onMounted, watch } from "@vue/runtime-core";
import FlvJs from "flv.js";

const props = defineProps<{
  video_src: string;
  volume: number;
  playing: boolean;
}>();

var flvPlayer: FlvJs.Player;
const videoElement = ref<HTMLVideoElement>();
const unSupported = ref<boolean>(false);

onMounted(() => {
  nextTick(() => {
    if (FlvJs.isSupported()) {
      flvPlayer = FlvJs.createPlayer({
        type: "flv",
        url: props.video_src,
      });
      flvPlayer.attachMediaElement(videoElement.value);
      flvPlayer.load();
      flvPlayer.volume = props.volume;
    } else {
      unSupported.value = true;
    }
  });
});

watch(
  () => props.video_src,
  () => {
    console.log(props.video_src);
    if (FlvJs.isSupported()) {
      flvPlayer = FlvJs.createPlayer({
        type: "flv",
        url: props.video_src,
        isLive: true,
        cors: true
      });
      flvPlayer.attachMediaElement(videoElement.value);
      flvPlayer.load();
    } else {
      unSupported.value = true;
    }
  }
);

watch(
  () => props.volume,
  () => {
    flvPlayer.volume = props.volume;
  }
);

watch(
  () => props.playing,
  () => {
    if(props.playing)
      flvPlayer.play();
    else
      flvPlayer.pause();

    console.log(props.video_src);
  }
);

onBeforeUnmount(() => {});
</script>

<template>
  <div class="w-100 h-100 m-auto" style="background-color: black;">
    <video class="w-100 m-0" style="min-height:100%; z-index: 1;" ref="videoElement"></video>
    <div
      class="position-absolute w-100 h-100 bg-dark"
      style="left: 0%; right: 0%; top: 0%; bottom: 0%; min-width: 100%; min-height: 100%; z-index: 5"
      v-if="unSupported"

    >
      <div class="d-flex flex-column justify-content-center">
        <h3 class="h3 text-light align-self-center">不支持该浏览器</h3>
      </div>
    </div>
  </div>
</template>