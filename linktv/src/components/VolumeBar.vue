<script setup lang="ts">
import { ref } from "@vue/reactivity";
import { nextTick, watch } from "@vue/runtime-core";

const props = defineProps({
  volume :{
    type: Number,
    required: true,
    default: 100
  },
}
);

const rangeValue = ref(props.volume);

const trackValue = ref({
  "background-size": props.volume+"% 100%",
});

const silent = ref(false);

const emites = defineEmits(["volumeChange"]);

watch(rangeValue, function () {
  trackValue.value["background-size"] = rangeValue.value + "% 100%";
  silent.value = rangeValue.value == 0;
  emites("volumeChange", rangeValue.value);
});

function onSilentSwitch(){
  silent.value = !silent.value;
  rangeValue.value = silent.value == false ? rangeValue.value : 0;
}

</script>

<template>
  <div class="volume-control">
    <button type="button" class="btn volume-btn" @click="onSilentSwitch">
      <svg
        t="1638282830550"
        class="icon fill-light"
        viewBox="0 0 1024 1024"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg"
        p-id="4100"
        width="1.2em"
        height="1.2em"
        v-if="!silent"
      >
        <path
          d="M128 420.576v200.864h149.12l175.456 140.064V284.288l-169.792 136.288H128z m132.256-64l204.288-163.968a32 32 0 0 1 52.032 24.96v610.432a32 32 0 0 1-51.968 24.992l-209.92-167.552H96a32 32 0 0 1-32-32v-264.864a32 32 0 0 1 32-32h164.256zM670.784 720.128a32 32 0 0 1-44.832-45.664 214.08 214.08 0 0 0 64.32-153.312 213.92 213.92 0 0 0-55.776-144.448 32 32 0 1 1 47.36-43.04 277.92 277.92 0 0 1 72.416 187.488 278.08 278.08 0 0 1-83.488 198.976zM822.912 858.88a32 32 0 1 1-45.888-44.608A419.008 419.008 0 0 0 896 521.152c0-108.704-41.376-210.848-114.432-288.384a32 32 0 0 1 46.592-43.872c84.16 89.28 131.84 207.04 131.84 332.256 0 127.84-49.76 247.904-137.088 337.728z"
          p-id="4101"
        ></path>
      </svg>
      <svg
        t="1638360948682"
        class="icon fill-light"
        viewBox="0 0 1024 1024"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg"
        p-id="7834"
        width="1.2em"
        height="1.2em"
        v-else
      >
        <path
          d="M128 420.576v200.864h149.12l175.456 140.064V284.288l-169.792 136.288H128z m132.256-64l204.288-163.968a32 32 0 0 1 52.032 24.96v610.432a32 32 0 0 1-51.968 24.992l-209.92-167.552H96a32 32 0 0 1-32-32v-264.864a32 32 0 0 1 32-32h164.256zM752 458.656L870.4 300.8a32 32 0 1 1 51.2 38.4L792 512l129.6 172.8a32 32 0 0 1-51.2 38.4l-118.4-157.856-118.4 157.856a32 32 0 0 1-51.2-38.4l129.6-172.8-129.6-172.8a32 32 0 0 1 51.2-38.4l118.4 157.856z"
          p-id="7835"
        ></path>
      </svg>
    </button>
    <div class="bg-dark volume-drop">
      <input type="range" :style="trackValue" v-model="rangeValue" />
    </div>
  </div>
</template>

<style scoped>
.volume-control:hover > .volume-drop {
  display: block;
}

input[type="range"] {
  -webkit-appearance: none;
  height: 10px;
  width: 80px;
  transform: translate(-25px, 30px) rotate(-90deg);
  outline: none;
  background: -webkit-linear-gradient(#2196f3, #2196f3) no-repeat, #343a40aa; /*背景颜色，俩个颜色分别对应上下，自己尝试下就知道了嗯*/
  background-size: 22% 100%;
  border: 1px solid #343a40aa;
  border-radius: 5px;
}

input[type="range"]::-webkit-slider-thumb {
  -webkit-appearance: none;
  height: 16px;
  width: 16px;
  background: #2196f3;
  border-radius: 50%;
  border: solid 1px #000000;
  box-shadow: 0px 0px 1px 2px #030202;
}

.volume-drop {
  display: none;
  position: absolute;
  border-style: solid;
  border-width: 1px;
  border-radius: 5px;
  left: 4.8em;
  bottom: 2.8em;
  width: 30px;
  height: 90px;
}
</style>