<script setup lang="ts">
import ImageUploadModal from "./ImageUploadModal.vue";
import JQuery from "jquery";
import { inject } from "@vue/runtime-core";
import { IUserStore } from "../Utils";

let userSore = inject<IUserStore>("userStore");

function onImageUploadCancel() {
  JQuery("#imageUploadModal").modal("hide");
}

function toggleLogoutConfirmModal(state: string) {
  JQuery("#logoutConfirmModal").modal(state);
}

function onLogout() {
  toggleLogoutConfirmModal("hide");
  userSore.logout();
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
            alt=""
            class="align-self-center img-fluid"
            style="width: 4em; height: 4em"
            :src="
              userSore.image != undefined
                ? userSore.image
                : './src/assets/user.png'
            "
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
            >
              <button
                type="button"
                class="btn text-light btn-sm"
                data-toggle="modal"
                data-target="#imageUploadModal"
              >
                编辑
              </button>
              <Teleport to="body">
                <div
                  class="modal fade"
                  tabindex="-1"
                  aria-hidden="true"
                  id="imageUploadModal"
                >
                  <ImageUploadModal
                    @close-event="onImageUploadCancel"
                  ></ImageUploadModal>
                </div>
              </Teleport>
            </div>
          </div>
        </div>
        <span class="align-self-center">{{ userSore.name }}</span>
        <table class="table table-borderless table-sm mt-3">
          <tbody>
            <tr>
              <td align="right">id:</td>
              <td>{{ userSore.id }}</td>
            </tr>
            <tr>
              <td align="right">email:</td>
              <td>{{ userSore.email }}</td>
            </tr>
          </tbody>
        </table>
      </div>
    </div>
    <div class="dropdown-divider"></div>
    <button
      class="btn dropdown-item text-center"
      data-toggle="modal"
      data-target="#logoutConfirmModal"
    >
      登出
    </button>

    <Teleport to="body">
      <div
        class="modal fade"
        tabindex="-1"
        aria-hidden="true"
        id="logoutConfirmModal"
      >
        <div class="modal-dialog">
          <div class="modal-content">
            <div class="modal-header">
              <button
                type="button"
                class="close"
                @click="toggleLogoutConfirmModal('hide')"
              >
                <span aria-hidden="true">&times;</span>
              </button>
            </div>
            <div class="modal-body">退出登录将无法享受某些功能，确定退出？</div>
            <div class="modal-footer">
              <button
                type="button"
                class="btn btn-secondary"
                @click="toggleLogoutConfirmModal('hide')"
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
    </Teleport>
  </div>
</template>

<style scoped>
.image-box {
  color: transparent;
  cursor: pointer;
}

.image-box:hover {
  background-color: #20202077;
  color: aliceblue;
  box-shadow: 0px 0px 5px #0f0f0fc0;
  transition: 0.3s;
}
</style>