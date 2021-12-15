<script setup lang="ts">
import { nextTick, onMounted } from "@vue/runtime-core";
import $ from "jquery";
import { ref } from "vue";

interface Props {
  msg: string
}

const props = withDefaults(defineProps<Props>(), {
  msg: "不知道什么原因"
});

const modalContainer = ref<HTMLDivElement>();

onMounted(() => {
  nextTick(() => {
  });
});
</script>

<template>
  <div
    aria-live="polite"
    aria-atomic="true"
    class="position-absolute"
    style="left: 0; top: 0; z-index: 2000;"
  >
    <div class="w-100 d-flex justify-content-center mt-4">
      <div
        id="errorToast"
        class="toast"
        role="alert"
        aria-live="assertive"
        aria-atomic="true"
        data-delay="5000"
      >
        <div class="toast-header bg-danger">
          <strong class="mr-auto text-light">错误</strong>
          <button
            type="button"
            class="ml-2 mb-1 close"
            data-dismiss="toast"
            aria-label="Close"
          >
            <span aria-hidden="true" class="text-light">&times;</span>
          </button>
        </div>
        <div class="toast-body">
          <svg
            t="1639449697618"
            class="icon mr-2"
            viewBox="0 0 1024 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="2325"
            width="1.5em"
            height="1.5em"
          >
            <path
              d="M512 0C229.376 0 0 229.376 0 512s229.376 512 512 512 512-229.376 512-512S794.624 0 512 0z m218.624 672.256c15.872 15.872 15.872 41.984 0 57.856-8.192 8.192-18.432 11.776-29.184 11.776s-20.992-4.096-29.184-11.776L512 569.856l-160.256 160.256c-8.192 8.192-18.432 11.776-29.184 11.776s-20.992-4.096-29.184-11.776c-15.872-15.872-15.872-41.984 0-57.856L454.144 512 293.376 351.744c-15.872-15.872-15.872-41.984 0-57.856 15.872-15.872 41.984-15.872 57.856 0L512 454.144l160.256-160.256c15.872-15.872 41.984-15.872 57.856 0 15.872 15.872 15.872 41.984 0 57.856L569.856 512l160.768 160.256z"
              fill="#CF3736"
              p-id="2326"
            ></path>
          </svg>
          {{ msg }}
        </div>
      </div>
    </div>
  </div>
</template>