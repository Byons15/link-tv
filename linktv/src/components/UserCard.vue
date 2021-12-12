<script setup lang="ts">
import { ref } from "@vue/reactivity";
import axios from "axios";
import { UserClient, UserDTO } from "../LinkClient";
import ImageUploadModal from "./ImageUploadModal.vue";

const name = ref("");
const id = ref(null);
const email = ref();

const emites = defineEmits(["logout"]);

const client = new UserClient();

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

const showImageUploadModal = ref(true);

function onShowImageUploadMoadal(){
  showImageUploadModal.value = true;
}

</script>

<template>
  <div>
    <div class="px-4 py-3">
      <div class="d-flex flex-column justify-content-center">
        <div
          class="
            d-flex
            align-self-center
            rounded-circle
            overflow-hidden
            bg-dark
          "
          style="height: 4em; width: 4em"
        >
          <img
            src="../assets/user.png"
            alt=""
            class="align-self-center img-fluid"
            style="width: 4em; height: 4em"
          />
          <div
            class="position-absolute"
            style="
              height: 4em;
              width: 4em;
              z-index: 2;
              background-color: transparent;
            "
          >
            <div
              class="
                w-100
                h-100
                rounded-circle
                d-flex
                flex-column
                justify-content-end
                image-box
                small
                text-center
              "
              @click="onShowImageUploadMoadal"
            >
              编辑
            </div>
          </div>
        </div>
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

    <ImageUploadModal v-if="showImageUploadModal"></ImageUploadModal>
  </div>
</template>

<style scoped>
.bg-translucent {
  z-index: 3;
  background-color: #20202077;
  left: 0%;
  top: 0%;
}

.image-box{
  color:transparent;
  cursor: pointer;
}

.image-box:hover {
  background-color: #20202077;
  color: aliceblue;
  box-shadow: 0px 0px 5px #0f0f0fc0;
  transition: 0.3s;
}
</style>