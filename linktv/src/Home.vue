<script setup>
import { ref } from "@vue/reactivity";
import Telecasts from "./components/Telecasts.vue";
import Navbar from "./components/navbar.vue";

import { nextTick, onMounted } from "@vue/runtime-core";

const playerWidescreen = ref(false);
const topTV = ref(true);

onMounted(()=>{
  nextTick(()=>{
    
  });
});

</script>

<template>
  <div>
    <Navbar></Navbar>
    <div class="container">
      <div class="row">
        <br>
      </div>
      <h4 class="row">正在直播</h4>
      <Telecasts></Telecasts>
    </div>
  </div>
</template>