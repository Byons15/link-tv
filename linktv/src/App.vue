<script setup lang="ts">
import { inject, onMounted, provide } from "@vue/runtime-core";
import { nextTick, ref } from "vue";
import ErrorModal from "./components/ErrorModal.vue";
import $ from "jquery";
import * as Utils from "./Utils";
import axios from "axios";

const globalErrorModal = ref(null);
const haveError = ref(false);
const errorMsg = ref("");

let globalUtils = {
  showError: null,
};

provide("globalUtils", globalUtils);

onMounted(() => {
  nextTick(() => {
    Utils.errorModal.value = globalErrorModal.value;
    axios.defaults.headers.common["Authorization"] =
      "Bearer " +
      (localStorage.getItem("token") != null
        ? localStorage.getItem("token")
        : "");
    Utils.onlineUser.token =
      localStorage.getItem("token") != null
        ? localStorage.getItem("token")
        : "";
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
