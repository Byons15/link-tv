<script setup>
import { ref } from "@vue/reactivity";
import { onMounted, nextTick } from "vue";

const showDanmaku = ref(true);
var darkTheme = ref(false);

const emits = defineEmits(["danmakuSwitch"]);

const parentRef = ref(null);

onMounted(() => {
  nextTick(() => {
    darkTheme.value = parentRef.value
      .getAttribute("class")
      .includes("dark-theme");
  });
});
</script>

<template>
  <div class="d-flex align-items-center" style="height: 2.5em" ref="parentRef">
    <div class="d-flex" style="height: 2.5m">
      <!--弹幕显示切换按钮-->
      <label class="switch" style="margin: 0 auto">
        <input
          type="checkbox"
          checked
          style="display: none"
          @change="$emit('danmakuSwitch', $event.target.checked)"
          v-model="showDanmaku"
        />
        <div class="slider-background"></div>
        <div class="slider d-flex justify-content-center">
          <div class="slider-content">弹</div>
        </div>
      </label>
    </div>

    <span class="d-flex align-items-center" v-if="showDanmaku">
      <!---设置上拉按钮-->
      <span class="dropup">
        <button
          class="btn"
          :class="{ 'll-btn-dark': darkTheme }"
          data-toggle="dropdown"
        >
          <svg
            t="1635490696343"
            class="icon"
            viewBox="0 0 1029 1024"
            version="1.1"
            xmlns="http://www.w3.org/2000/svg"
            p-id="2926"
            width="1.5em"
            height="1.5em"
            :class="{ 'fill-light': darkTheme }"
          >
            <path
              d="M809.472 786.432c48.64 0 88.576-39.936 88.576-88.576s-39.936-88.576-88.576-88.576-88.576 39.936-88.576 88.576 39.936 88.576 88.576 88.576z m-45.056-88.576c0-24.576 19.968-45.056 45.056-45.056s45.056 19.968 45.056 45.056c0 24.576-19.968 45.056-45.056 45.056s-45.056-19.968-45.056-45.056z"
              p-id="2927"
            ></path>
            <path
              d="M1016.832 735.232c-18.944-21.504-18.944-54.272 0-74.752 12.288-13.312 15.872-32.768 9.216-49.664-7.68-19.968-19.456-38.4-30.72-55.296-10.24-13.824-28.672-20.992-47.104-16.896-13.312 3.072-27.136 1.024-39.936-6.144-12.288-6.656-21.504-18.432-25.088-31.232-5.632-17.408-19.968-29.696-38.4-32.768-20.992-2.56-42.496-2.56-62.976 0-17.92 3.072-32.768 15.36-38.4 32.768-3.584 9.728-10.752 23.04-25.088 31.232-12.288 7.168-26.112 9.216-39.936 6.144-3.072-0.512-6.656-1.024-9.728-1.024-14.848 0-28.672 6.656-37.888 17.92-13.312 16.896-24.064 35.84-31.744 54.784-6.656 16.896-3.072 36.352 9.216 49.664 18.944 21.504 18.944 54.272 0 74.752-12.288 13.312-15.872 32.768-9.216 49.664 6.656 17.408 17.408 35.328 31.744 54.784 11.264 13.824 29.184 20.48 46.592 16.896 4.096-1.024 8.192-1.536 12.288-1.536 24.576 0 45.568 15.36 52.736 38.912 5.632 17.408 19.968 29.696 38.4 32.768 10.24 1.536 20.992 2.56 31.744 2.56 10.24 0 20.992-1.024 31.744-1.536 17.92-3.072 32.768-15.36 38.4-32.768 3.584-9.728 10.752-23.04 25.088-31.232 12.288-7.168 26.112-9.216 39.936-6.144 3.072 0.512 6.144 1.024 9.728 1.024 14.336 0 28.16-6.656 37.376-17.92 13.312-16.896 24.064-35.84 31.744-54.784 6.144-17.408 2.048-36.864-9.728-50.176z m-351.744 76.288c-11.264-14.848-19.456-29.184-25.088-43.52 0-0.512-0.512-2.048 0.512-3.072 34.304-37.888 34.304-95.744 0-133.632-0.512-0.512-1.024-1.024-0.512-4.096 6.144-14.336 14.848-28.672 25.6-43.008 0.512-0.512 1.536-1.024 2.56-1.024h0.512c24.576 5.12 50.688 1.536 71.168-10.752 22.016-12.288 37.888-32.256 45.056-56.832 0-0.512 0.512-1.536 2.56-2.048 16.896-2.56 33.792-2.56 50.688 0h0.512c1.024 0 1.536 1.536 1.536 2.048 7.168 24.064 23.04 44.032 45.056 56.32 20.48 12.288 46.08 15.872 70.656 10.752 1.024-0.512 2.56-0.512 3.072 0.512 10.24 12.288 18.944 27.648 25.6 43.52 0.512 3.072 0 3.584-0.512 4.096-34.304 37.888-34.304 95.744 0 133.632 0.512 0.512 1.024 2.56 0.512 3.584-6.144 14.336-14.848 29.184-25.6 43.52-0.512 0.512-1.536 1.024-2.56 1.024h-0.512c-24.576-5.12-50.688-1.536-71.168 10.752-22.016 12.288-37.888 32.256-45.056 56.832 0 0.512-0.512 1.536-2.56 2.048h-1.536c-8.192 0.512-15.872 1.536-24.064 1.536s-16.896-1.024-25.088-1.536c-2.048-0.512-2.56-1.536-2.56-2.56-13.312-40.96-52.224-69.12-94.72-69.12-7.168 0-13.824 1.024-20.992 2.048-1.024 0-2.56-0.512-3.072-1.024zM356.864 305.152h444.416v64H356.864zM509.44 705.536c15.872-16.896 22.528-39.936 19.968-61.952H332.288v64h175.616c0.512-1.024 1.024-1.536 1.536-2.048zM543.744 487.424c4.608-5.632 9.728-9.728 15.872-13.312H220.672v64H512c8.704-17.92 19.968-35.328 31.744-50.688z"
              p-id="2928"
            ></path>
            <path
              d="M543.744 849.408c-5.632-7.68-11.264-15.36-16.384-23.552H223.232c-54.272 0-96.256-41.472-96.256-96.256V282.112c0-54.272 41.472-96.256 96.256-96.256h576c54.272 0 96.256 41.472 96.256 96.256v175.104c1.024 0.512 2.048 1.536 3.584 2.048 17.408 9.728 36.864 13.312 56.832 8.704 1.536-0.512 2.56-0.512 4.096-0.512V282.112c0-89.6-70.656-159.744-159.744-159.744H223.232c-89.6 0-159.744 70.656-159.744 159.744v448c0 89.6 70.144 159.744 159.744 159.744h357.888c-18.432-17.92-24.064-24.064-37.376-40.448z"
              p-id="2929"
            ></path>
          </svg>
        </button>

        <!----设置-->
        <div class="dropdown-menu bg-dark rounded"></div>
      </span>

      <!--弹幕输入和发送按钮-->
      <div class="input-group d-flex align-items-center">
        <span class="input-group-prepend">
          <button class="btn">A</button>
        </span>
        <input type="text" class="form-control" />
        <span class="input-group-append">
          <button
            class="btn"
            :class="{ 'text-light': darkTheme }"
            style="width: 4em"
          >
            发送
          </button>
        </span>
      </div>
    </span>
  </div>
</template>

<style scoped>
.switch {
  position: relative;
  width: 2.4em;
  height: 1.5em;
}

input {
  color: #333333;
}

input:focus {
  border-style: none;
  outline: none;
}

.slider-background {
  position: absolute;
  width: 100%;
  height: 100%;
  background-color: #ccc;
  border-radius: 5em;
  -webkit-transition: 0.4s;
  transition: 0.4s;
}

.slider {
  width: 1.1em;
  height: 1.1em;
  background-color: white;
  position: absolute;
  left: 0.2em;
  top: 0.2em;
  bottom: 0%;
  right: 0%;
  border-radius: 5em;
  color: #ccc;
  -webkit-transition: 0.4s;
  -moz-transition: 0.4s;
  transition: 0.4s;
}

.danmaku-controller {
  display: table-cell;
  vertical-align: middle;
}

.switch {
  cursor: pointer;
}

input:checked ~ .slider-background {
  background-color: #2196f3;
}

input:checked ~ .slider {
  -moz-transform: translateX(0.9em);
  -webkit-transform: translateX(0.9em);
  transform: translateX(0.9em);
  color: #2196f3;
}

.slider-content {
  font-size: 0.5em;
  user-select: none;
  -webkit-user-select: none;
  -moz-user-select: none;
}
</style>

<style>
.dark-theme {
  height: 2.5em;
}
</style>
