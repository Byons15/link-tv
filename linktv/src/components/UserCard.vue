<script setup lang="ts">
import { ref } from "@vue/reactivity";
import axios from "axios";
import { UserClient, UserDTO } from "../LinkClient";

const name = ref("");
const id = ref(null);
const email = ref();

const emites = defineEmits(["logout"]);

const client = new UserClient();

// userApi.apiUserGet().then((response) => {
//   name.value = response.data.name;
//   id.value = response.data.id;
//   email.value = response.data.email;
// });

client.get(0).then((user: UserDTO) => {
  name.value = user.name;
  id.value = user.id;
  email.value = user.email;
});

function onLogout() {
  localStorage.removeItem("token");
  axios.defaults.headers.common["Authorization"] = "";
  emites("logout");
}

const logoutConfirm = ref(false);

function showLogoutConfirm() {
  logoutConfirm.value = true;
}
</script>

<template>
  <div>
    <div class="px-4 py-3">
      <div class="d-flex flex-column justify-content-center">
        <svg
          t="1636988377253"
          class="icon align-self-center"
          viewBox="0 0 1024 1024"
          version="1.1"
          xmlns="http://www.w3.org/2000/svg"
          p-id="2875"
          width="4em"
          height="4em"
        >
          <path
            d="M761.8 600.9c-64 57.6-147.2 96-243.2 96s-179.2-38.4-249.6-96C108.8 684.2 0 793 0 991.4H1024.2c0-198.4-108.8-300.8-262.4-390.5z m-249.7 19.2c160 0 288.1-134.4 288.1-294.5 0-160-128-294.5-288.1-294.5s-288 134.6-288 294.6c0 166.4 134.4 294.4 288 294.4z m0 0"
            fill="#8a8a8a"
            p-id="2876"
          ></path>
        </svg>
        <span class="align-self-center">{{ name }}</span>
        <table class="table table-borderless table-sm mt-3">
          <tbody>
            <tr>
              <td align="right">id:</td>
              <td>{{ id }}</td>
            </tr>
            <tr>
              <td align="right">email:</td>
              <td>{{ email }}</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
    <div class="dropdown-divider"></div>
    <button class="btn dropdown-item text-center" @click="showLogoutConfirm">
      登出
    </button>

    <!-- Modal -->
    <div class="position-fixed w-100 h-100 bg-translucent" v-if="logoutConfirm">
      <div class="modal-dialog">
        <div class="modal-content">
          <div class="modal-header">
            <button type="button" class="close" @click="logoutConfirm = false">
              <span aria-hidden="true">&times;</span>
            </button>
          </div>
          <div class="modal-body">退出登录将无法享受某些功能，确定退出？</div>
          <div class="modal-footer">
            <button
              type="button"
              class="btn btn-secondary"
              @click="logoutConfirm = false"
            >
              取消
            </button>
            <button type="button" class="btn btn-primary" @click="onLogout">
              确定
            </button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.bg-translucent {
  z-index: 3;
  background-color: #20202077;
  left: 0%;
  top: 0%;
}
</style>