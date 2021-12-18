<script setup lang="ts">
import { ref } from "@vue/reactivity";
import Router from "../Router";
import axios, { AxiosResponse } from "axios";
import { inject, nextTick, onBeforeUpdate, onMounted } from "@vue/runtime-core";
import {
  IInternalErrorDescription,
  IInvalidModelDescription,
  Login,
  UserClient,
} from "../LinkClient";
import * as Utils from "../Utils";

const userClient = new UserClient();

const userName = ref("");
const password = ref("");

const userStore = inject<Utils.IUserStore>("userStore");
const errorModal = inject<any>("errorModal");

onMounted(() => {
  nextTick(() => {
    if(userStore.logined)
      throw "使用登录面板前请先注销";
  });
});

onBeforeUpdate(() => {});

function onSignup() {
  Router.push("/signup");
}

const logging = ref(false);
const invalidName = ref(false);
const invalidPassword = ref(false);
const invalidNameDesc = ref<string>();
const invalidPasswordDesc = ref<string>();

function onLogin() {
  logging.value = true;
  userClient
    .login(<Login>{
      name: userName.value,
      password: password.value,
    })
    .then((response: string) => {
      userStore.login(response);
    })
    .catch((response: IInvalidModelDescription) => {
      if (response.errors["Name"] !== undefined) {
        invalidName.value = true;
        invalidNameDesc.value = response.errors["Name"][0];
      }

      if (response.errors["Password"] !== undefined) {
        invalidPassword.value = true;
        invalidPasswordDesc.value = response.errors["Password"][0];
      }
    })
    .catch((error) => {
      console.log(error);
      errorModal.value.show();
    })
    .finally(() => {
      logging.value = false;
    });
}
</script>

<template>
  <form
    class="px-4 py-3 d-flex flex-column justify-content-center needs-validation"
    novalidate
    style="min-width: 20em"
  >
    <div class="form-row">
      <label for="userInputBox">用户</label>
      <input
        type="text"
        class="form-control"
        v-model="userName"
        id="userInputBox"
        required
        :class="{ 'is-invalid': invalidName }"
        @change="invalidName = false"
      />
      <div class="invalid-feedback">
        {{ invalidNameDesc }}
      </div>
      <div class="valid-feedback"></div>
    </div>
    <div class="form-row">
      <label for="userPasswordInputBox">密码</label>
      <input
        type="password"
        class="form-control"
        v-model="password"
        id="userPasswordInputBox"
        required
        :class="{ 'is-invalid': invalidPassword }"
        @change="invalidPassword = false"
      />
      <div class="invalid-feedback">
        {{ invalidPasswordDesc }}
      </div>
      <div class="valid-feedback"></div>
    </div>

    <div class="d-flex flex-row justify-content-between">
      <a class="form-text" href="javascript:;">忘记密码？</a>
      <a class="form-text" href="javascript:;" @click="onSignup">注册</a>
    </div>
    <br />
    <button
      type="submit"
      class="btn btn-primary w-25"
      @Click="onLogin"
      :disabled="logging"
    >
      <span v-if="!logging">登录</span>
      <div class="spinner-border text-light small" role="status" v-else>
        <span class="sr-only">Loading...</span>
      </div>
    </button>
  </form>
</template>
