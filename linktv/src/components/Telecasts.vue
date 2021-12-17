<script setup lang="ts">
import TVCard from "./TVCard.vue";
import { inject, nextTick, onMounted } from "@vue/runtime-core";
import { ref } from "vue";
import { LiveClient, Studio } from "../LinkClient";

const lives = ref<Studio[]>();
const empty = ref(false);

const errorModal = inject<any>("errorModal");

onMounted(() => {
  nextTick(() => {
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