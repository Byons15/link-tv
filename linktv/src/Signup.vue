<script setup lang="ts">
import { ref } from "@vue/reactivity";
import Router from "./Router";
import {
  IInvalidModelDescription,
  Signup,
  UserClient,
  UserCore,
} from "./LinkClient";
import axios from "axios";

const userName = ref("");
const userNameInvalidMsg = ref("");
const email = ref("");
const emailInvalidMsg = ref("");
const emailAuthCode = ref("");
const authCodeInvalidMsg = ref("");
const password = ref("");
const repeatPassword = ref("");
const passwordInvalidMsg = ref("");
const repeatPasswordInvalidMsg = ref("");

// const api = new UserApi();
// const captchaApi = new CaptchaApi();

function onUserNameBlur() {
  if (userName.value.length == 0) {
    userNameInvalidMsg.value = "";
    return;
  }

  let userClient = new UserClient();
  userClient.validName(userName.value).then((available: boolean) => {
    userNameInvalidMsg.value = available ? "" : "用户名已经存在";
  });
}

function onEmailBlur() {
  if (email.value.length == 0) {
    emailInvalidMsg.value = "";
    return;
  }

  let userClient = new UserClient();
  userClient
    .validEmail(email.value)
    .then((availdable) => {
      emailInvalidMsg.value = availdable ? "" : "该邮箱已被注册";
    })
    .catch((invalidEmail: IInvalidModelDescription) => {
      emailInvalidMsg.value = invalidEmail.errors.email[0];
    });
}

const actionLimit = ref(false);
const sendBtnContent = ref("验证");
var repeatCountdown = 120;

function sendEmailAuthCode() {
  if (emailInvalidMsg.value != "" || email.value == "") return;

  const client = new UserClient();
  client
    .sendGuestEmailAuthCode(email.value)
    .then(() => {
      actionLimit.value = true;
      repeatCountdown = 120;
      sendBtnContent.value = repeatCountdown.toString();
      let timer = setInterval(() => {
        --repeatCountdown;
        sendBtnContent.value = repeatCountdown.toString();
        if(repeatCountdown == 0){
          clearInterval(timer);
          sendBtnContent.value = "验证";
        }
      }, 1000);
    })
    .catch((invalidModel: IInvalidModelDescription) => {
      emailInvalidMsg.value = invalidModel.errors.email[0];
    });
}

function onEmailAuthCodeBlur() {
}

function onPasswordBlur() {
  if (password.value.length == 0) {
    passwordInvalidMsg.value = "";
    return;
  }

  var pattern = /^(?![0-9]+$)(?![a-zA-Z]+$)[0-9A-Za-z_]{8,128}$/;
  passwordInvalidMsg.value = pattern.test(password.value)
    ? ""
    : "密码必须以字母开头，最少8位";
}

function onRepeatPasswordBlur() {
  if (
    passwordInvalidMsg.value != "" &&
    (password.value.length == 0 || repeatPassword.value.length == 0)
  ) {
    repeatPasswordInvalidMsg.value = "";
    return;
  }

  repeatPasswordInvalidMsg.value =
    password.value == repeatPassword.value ? "" : "密码不一致";
}

function submit() {
  onRepeatPasswordBlur();
  if(repeatPasswordInvalidMsg.value.length > 0){
    return;
  }

  const client = new UserClient();
  client
    .signup(<Signup>{
      name: userName.value,
      email: email.value,
      emailAuthCode: emailAuthCode.value,
      password: password.value,
    })
    .then((token: string) => {
      localStorage.setItem("token", token);
      axios.defaults.headers.common["Authorization"] =
        "Bearer " + localStorage.getItem("token");
      Router.push("/home");
    })
    .catch((invalidResponse: IInvalidModelDescription) => {
      const errors = invalidResponse.errors;

      userNameInvalidMsg.value = 
        errors["Name"] !== undefined ? errors['Name'][0] : '';

      emailInvalidMsg.value = 
        errors['Email'] !== undefined ? errors['Email'][0] : '';

      authCodeInvalidMsg.value = 
        errors["EmailAuthCode"] !== undefined ? errors["EmailAuthCode"][0] : '';

      passwordInvalidMsg.value =
        errors["Password"] !== undefined ? errors["Password"][0] : '';
    });
}

function onClose() {
  Router.push("/home");
}
</script>

<template>
  <div class="position-fixed w-100 h-100 bg-translucent">
    <div class="d-flex w-100 h-100 justify-content-center">
      <div
        class="
          align-self-center
          d-flex
          flex-column
          justify-content-center
          bg-light
          px-5
          py-4
          rounded
        "
      >
        <button
          type="button"
          class="close align-self-end"
          aria-label="Close"
          @click="onClose"
        >
          <span aria-hidden="true">&times;</span>
        </button>

        <div>
          <label for="user-name" class="mb-0 mt-3">用户名</label>
          <input
            id="user-name"
            class="form-control"
            type="text"
            v-model="userName"
            @blur="onUserNameBlur"
            :class="userNameInvalidMsg == '' ? '' : 'is-invalid'"
          />
          <div class="invalid-feedback">{{ userNameInvalidMsg }}</div>
        </div>
        <div class="">
          <label for="email" class="mb-0 mt-3 form-text">Email</label>
          <div class="input-group">
            <input
              id="email"
              type="text"
              class="form-control"
              v-model="email"
              @blur="onEmailBlur"
              :class="emailInvalidMsg == '' ? '' : 'is-invalid'"
            />
            <span class="input-group-append">
              <button
                class="btn"
                @click="sendEmailAuthCode"
                :disabled="actionLimit"
              >
                {{sendBtnContent}}
              </button>
            </span>
            <div class="invalid-feedback">
              {{ emailInvalidMsg }}
            </div>
          </div>
        </div>
        <div>
          <label for="email-authcode" class="mb-0 mt-3 form-text"
            >Email验证码</label
          >
          <input
            id="email-authcode"
            type="text"
            class="form-control"
            v-model="emailAuthCode"
            @blur="onEmailAuthCodeBlur"
            :class="authCodeInvalidMsg == '' ? '' : 'is-invalid'"
          />
          <div class="invalid-feedback">{{ authCodeInvalidMsg }}</div>
        </div>
        <div>
          <label for="password" class="mb-0 mt-3">密码</label>
          <input
            id="password"
            class="form-control"
            type="password"
            v-model="password"
            @blur="onPasswordBlur"
            :class="passwordInvalidMsg == '' ? '' : 'is-invalid'"
          />
          <div class="invalid-feedback">{{ passwordInvalidMsg }}</div>
        </div>
        <div>
          <label for="repeat-password" class="mb-0 mt-3">重复密码</label>
          <input
            id="repeat-password"
            class="form-control"
            type="password"
            v-model="repeatPassword"
            @blur="onRepeatPasswordBlur"
            :class="repeatPasswordInvalidMsg == '' ? '' : 'is-invalid'"
          />
          <div class="invalid-feedback">{{ repeatPasswordInvalidMsg }}</div>
        </div>
        <br />
        <button class="btn btn-primary w-25" @click="submit">注册</button>
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

.rounded {
  border-radius: 1em !important;
  max-width: 23em;
  min-width: 23em;
}
</style>