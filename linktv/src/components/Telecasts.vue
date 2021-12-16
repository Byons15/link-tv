<script setup lang="ts">
import TVCard from "./TVCard.vue";
import { nextTick, onMounted } from "@vue/runtime-core";
import { ref } from "vue";
import { LiveClient, Studio } from "../LinkClient";
import { errorModal } from "../Utils";

const lives = ref<Studio[]>();
const empty = ref(false);

onMounted(() => {
  nextTick(() => {
    // const liveApi = new LiveApi();
    // liveApi.apiLiveGet().then((response) => {
    //   if (response.status === 200) {
    //     lives.value = response.data ;
    //     empty.value = lives.value.length == 0 ? true : false;
    //   }
    // });
    const liveClient = new LiveClient();
    liveClient.all().then((studios: Studio[])=>{
      lives.value = studios;
       empty.value = lives.value.length == 0 ? true : false;
    }).catch(()=>{
      errorModal.value.show();
    });

  });
});
</script>

<template>
  <div class="row">
    <!-- <TVCard
      class="col-3"
      title="又是冲分的一天"
      image_src="./src/assets/003.flv"
      anchor_image_src="./src/assets/dy1.jfif"
      anchor="旭旭宝宝"
      attendance="2000"
    ></TVCard> -->
    <div class='col-12 text-center' v-if="empty">什么都没有~~~</div>
    <TVCard
      v-for="(tv, i) in lives"
      class="col-3"
      :liveId="tv.id"
      attendance="2000"
      :playAddress="tv.flvAddress"
      :title="tv.name"
      :director="tv.director"
      :key="i"
      v-else
    ></TVCard>
  </div>
</template>

<style scoped>
</style>