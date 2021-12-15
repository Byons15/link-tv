<script setup lang="ts">
import { inject, onMounted, provide } from "@vue/runtime-core";
import { nextTick, ref } from "vue";
import ErrorModal from "./components/ErrorModal.vue";
import $ from "jquery";

const globalErrorModal = ref(null);
const haveError = ref(false);
const errorMsg = ref("");

let globalUtils = {
  showError: null
}

provide("globalUtils", globalUtils);

onMounted(()=>{
  nextTick(()=>{
    globalUtils.showError = (msg:string)=>{
      errorMsg.value = msg;
      $("#global-error-modal").toast('show');
      $("#global-error-modal").on("hidden.bs.toast", () =>{
        haveError.value = false;
      });
    }
    });
});

</script>

<template>
  <div>
    <router-view></router-view>
    <ErrorModal :msg="errorMsg" id="global-error-modal"></ErrorModal>
  </div>
</template>
<style>
</style>
