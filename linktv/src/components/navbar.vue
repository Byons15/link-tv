<script setup>
import { ref } from "@vue/reactivity";
import Router from "../Router";
import Login from "./Login.vue";
import UserCard from "./UserCard.vue";
import LiveGuide from "./LiveGuide.vue";
import { nextTick, onMounted } from "@vue/runtime-core";

const emites = defineEmits(["logined"]);

const logined = ref(false);

onMounted(()=>{
  nextTick(()=>{
    let token = localStorage.getItem("token");
    if(token != null){
      logined.value = true;
      emites("logined");
    }
  });
});

function onLogined() {
  emites("logined");
  logined.value = true;
}

function onLogout() {
  logined.value = false;
}

</script>

<template>
  <nav class="navbar navbar-expand-sm navbar-light">
    <a class="navbar-brand" href="javascript:;">
      <strong>Link TV</strong>
    </a>
    <button
      class="navbar-toggler"
      type="button"
      data-toggle="collapse"
      data-target="#navbarToggle"
      aria-controls="navbarToggle"
      aria-expanded="false"
      aria-label="Toggle navigation"
    >
      <span class="navbar-toggler-icon"></span>
    </button>
    <div id="navbarToggle" class="collapse navbar-collapse">
      <ul class="navbar-nav mr-auto">
        <li class="nav-item">
          <form class="form-inline">
            <input class="form-control" />
            <button class="btn">查找</button>
          </form>
        </li>
      </ul>

      <!--历史记录-->
      <button type="button" class="btn">
        <svg
          t="1634997811697"
          class="icon"
          viewBox="0 0 1024 1024"
          version="1.1"
          xmlns="http://www.w3.org/2000/svg"
          p-id="6372"
          width="1.5em"
          height="1.5em"
        >
          <path
            d="M511.73808594 93.19941406c-140.67070313 0-264.92695313 69.1453125-341.01035156 175.08955078v-70.47597656c0-21.66767578-17.61679688-39.22998047-39.346875-39.22998047s-39.346875 17.56318359-39.346875 39.22998047v156.91992188c0 21.66240234 17.61679688 39.22998047 39.346875 39.22998047h170.50341796c21.73007813 0 39.346875-17.56669922 39.346875-39.22998047 0-21.66767578-17.61679688-39.22998047-39.346875-39.22998047h-68.4421875c61.73085937-86.98798828 163.28496094-143.84267578 278.296875-143.84267578 188.32851562 0 341.00507812 152.22128906 341.00507813 339.99345703 0 187.76865234-152.6765625 339.99345703-341.00507813 339.99345703-158.73574219 0-291.74589844-108.29179688-329.75771484-254.70439453l-0.68291016 0.21269531c-5.3015625-15.41162109-19.54248047-26.65986328-36.8024414-26.65986328-21.73007813 0-39.346875 17.56318359-39.346875 39.22558594 0 3.86630859 1.16367187 7.36875 2.20605469 10.89316406l-0.40957032 0.12919922c0.25839844 0.95009766 0.73212891 1.8 0.99580078 2.74482422 0.28916016 0.76201172 0.48164063 1.52314453 0.81650391 2.2640625 49.92539062 175.47011719 210.98935547 304.35380859 402.98378906 304.35380859 231.79394531 0 419.69970703-187.35029297 419.69970703-418.45341797-0.00175781-231.10751953-187.90839844-418.45429688-419.70322265-418.45429687z m0 176.53183594c-21.73007813 0-39.346875 17.56318359-39.346875 39.22998047v235.37988281c0 21.66767578 17.61679688 39.22998047 39.346875 39.22998047h170.50253906c21.73007813 0 39.346875-17.56318359 39.346875-39.22558594 0-21.67119141-17.61679688-39.22998047-39.346875-39.22998047H551.08496094v-196.15429687c0-21.66767578-17.61767578-39.22998047-39.34775391-39.22998047z"
            p-id="6373"
          />
        </svg>
      </button>

      <!--开播指引-->
      <div class="dropdown">
        <button
          type="button"
          class="btn"
          role="button"
          data-toggle="dropdown"
          aria-expanded="false"
        >
          <svg
            t="1634997536655"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="4087"
            width="1.5em"
            height="1.5em"
          >
            <path
              d="M800 330.9V256c0-35.3-28.7-64-64-64H64c-35.3 0-64 28.7-64 64v512c0 35.3 28.7 64 64 64h672c35.3 0 64-28.7 64-64v-74.9c0-11.6 11.9-19.3 22.5-14.6L934 728c42.3 18.8 90-12.2 90-58.5v-315c0-46.3-47.7-77.3-90-58.5l-111.5 49.6c-10.6 4.7-22.5-3.1-22.5-14.7zM720 768H80c-8.8 0-16-7.2-16-16V272c0-8.8 7.2-16 16-16h640c8.8 0 16 7.2 16 16v480c0 8.8-7.2 16-16 16z m217.5-108.5l-128-56.9c-5.8-2.6-9.5-8.3-9.5-14.6V436c0-6.3 3.7-12.1 9.5-14.6l128-56.9c10.6-4.7 22.5 3 22.5 14.6v265.8c0 11.6-11.9 19.3-22.5 14.6z"
              p-id="4088"
            />
            <path
              d="M332.4 445.4L447.9 512l-115.4 66.6V445.4m-16-75.8c-25.1 0-48.1 20-48.1 48.1v188.7c0 28 23 48.1 48.1 48.1 8 0 16.2-2 23.9-6.5l163.4-94.4c32-18.5 32-64.7 0-83.1l-163.4-94.4c-7.7-4.5-15.9-6.5-23.9-6.5z"
              p-id="4089"
            />
          </svg>
        </button>
        <div
          class="dropdown-menu dropdown-menu-right p-4"
          style="min-width: 20em"
        >
          <LiveGuide :logined="logined"></LiveGuide>
        </div>
      </div>

      <!--用户中心-->
      <div class="dropdown">
        <button
          class="btn"
          id="userDropdown"
          role="button"
          data-toggle="dropdown"
          aria-expanded="false"
        >
          <svg
            t="1634873118556"
            class="icon"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="2728"
            width="1.5em"
            height="1.5em"
          >
            <path
              d="M500 128.8c-95.2 5.6-173.6 83.2-180 178.4-7.2 112 80.8 205.6 191.2 205.6 106.4 0 192-86.4 192-192 0.8-110.4-92-198.4-203.2-192zM512 575.2c-128 0-383.2 64-383.2 192v96c0 17.6 14.4 32 32 32h702.4c17.6 0 32-14.4 32-32V766.4c0-127.2-255.2-191.2-383.2-191.2z"
              p-id="2729"
            />
          </svg>
        </button>
        <div class="dropdown-menu dropdown-menu-right">
          <Login @logined="onLogined" @logout="onLogout" v-if="!logined"></Login>
          <UserCard v-else @logout-event="onLogout"></UserCard>
        </div>
      </div>
    </div>
    <div>
      
    </div>
  </nav>
</template>

<style>
</style>
    