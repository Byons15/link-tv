<script setup lang="ts">
import { computed, ref } from "@vue/reactivity";
import { watch, watchEffect } from "@vue/runtime-core";
import { LiveClient } from "../LinkClient";
import "../Utils";
import { errorModal } from "../Utils";

const props = defineProps({
  logined: Boolean,
});

const pushUrl = ref("");

watch(
  () => props.logined,
  () => {
    if (props.logined) {
      // const api = new LiveApi();
      // api.apiLiveAddressPushGet().then((response) => {
      //   if (response.status != 200) return;
      //   pushUrl.value = response.data;
      // });
      const live = new LiveClient();
      live.pushAddress(0)
      .catch(()=>{
        errorModal.value.show();
      })
      .then((address: string)=>{
        pushUrl.value = address;
      });
    } else {
      pushUrl.value = "";
    }
  }
);

const pushUrlElement = ref(null);

function onCopyPushUrl() {
  pushUrlElement.value.select();
  document.execCommand("Copy");
  console.log("复制成功");
}
</script>

<template>
  <div class="h5">开始你的Live</div>
  <div class="dropdown-divider mt-4 mb-3"></div>
  <div class="text-secondary mb-2">从专属于 Link TV 的推流器开始。</div>
  <button class="btn btn-primary btn-sm">下载Linkme</button>
  <div class="my-2"><br /></div>
  <div class="text-secondary">从其他推流软件，如OBS开始。</div>
  <div class="text-danger" v-if="pushUrl == ''">登录以获取推流地址</div>
  <div v-else>
    <div class="text-secondary mb-2">复制以下推流地址到推流软件：</div>
    <div class="input-group">
      <input
        type="text"
        readonly="true"
        v-model="pushUrl"
        class="form-control"
        ref="pushUrlElement"
      />
      <span class="input-group-append">
        <button
          type="button"
          class="btn btn-outline-secondary btn-sm"
          @click="onCopyPushUrl"
        >
          复制
        </button>
      </span>
    </div>
    <div class="small text-secondary">该地址仅24小时内有效。</div>
  </div>
</template>