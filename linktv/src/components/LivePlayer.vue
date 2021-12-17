<script setup>
import elementResizeDetector from "element-resize-detector/dist/element-resize-detector";
import flvjs from "flv.js";
import Danmaku from "danmaku";
import DanmakuController from "./danmakuController.vue";
import { ref } from "@vue/reactivity";
import { onMounted, onBeforeUnmount, nextTick } from "@vue/runtime-core";
import VolumeBar from "./VolumeBar.vue";
import FlvPlayer from "./FlvPlayer.vue";

// props
const props = defineProps({
  title: String,
  anchor: String,
  video_src: String,
  display_mode: String,
  danmakus: [],
});

// emits
const emits = defineEmits(["switchWidescreen"]);

// private values
var danmakuView = null;
var erd = elementResizeDetector();

// bind values
const playing = ref(false);
const fullScreen = ref(false);
const hideDanmaku = ref(false);
const widescreen = ref(false);
const pageFullscreen = ref(false);
const controlMouseHover = ref(true);
const volume = ref(100);

// refs values
const playerContainerElement = ref(null);
const danmakuContainerElement = ref(null);

onMounted(() => {
  nextTick(() => {
    // 弹幕构建
    danmakuView = new Danmaku({
      container:danmakuContainerElement.value,
      comments: [],
    });

    // 监听播放器容器大小
    erd.listenTo(playerContainerElement.value, () => {
      // 实时更新video标签的大小
      // 因为videoElementSize只有在网页全屏状态下才会绑定到video的style
      // 所以，以下代码是等效的
      // if(pageFullscreen.value){
      //   videoElementSize.width = element.clientWidth + "px";
      //   videoElementSize.height = element.clientHeight + "px";
      // }
      // 在下一时刻再重置弹幕系统大小
      // 因为我也不知道跑到这的时候，danmakuContainer的属性还有没有改变
      nextTick(() => {
        danmakuView.resize();
      });
    });

    //监听document事件以在用户使用快捷键或其他方式退出全屏时保持fullScreen的正确
    window.addEventListener("fullscreenchange", () => {
      let isFullScreen =
        document.fullScreen ||
        document.mozFullScreen ||
        document.webkitIsFullScreen;
      if (!isFullScreen) {
        fullScreen.value = false;
      }
    });
  });
});

function onVolumeChange(newVol){
  volume.value = Number(newVol);
};

onBeforeUnmount(() => {
  erd.uninstall();
});

function onPause() {
  playing.value = false;
}

// 宽屏处理交由父组件处理，父组件更改这个组件和其他组件的布局以实现宽屏处理
function onSwitchWidescreen(state) {
  emits("switchWidescreen", state);
  widescreen.value = state;
}

// 直接调用document处理全屏切换
function onFullScreenSwitch(full) {
  fullScreen.value = full;

  if (full) {
    if (playerContainerElement.value.requestFullscreen) {
      playerContainerElement.value.requestFullscreen();
    } else if (playerContainerElement.value.mozRequestFullScreen) {
      playerContainerElement.value.mozRequestFullScreen();
    } else if (playerContainerElement.value.msRequestFullscreen) {
      playerContainerElement.value.msRequestFullscreen();
    } else if (playerContainerElement.value.oRequestFullscreen) {
      playerContainerElement.value.oRequestFullscreen();
    }
  } else {
    if (document.exitFullscreen) {
      document.exitFullscreen();
    } else if (document.msExitFullscreen) {
      document.msExitFullscreen();
    } else if (document.mozCancelFullScreen) {
      document.mozCancelFullScreen();
    } else if (document.oRequestFullscreen) {
      document.oCancelFullScreen();
    } else if (document.webkitExitFullscreen) {
      document.webkitExitFullscreen();
    }
  }
}

function onPlay() {
  playing.value = true;
}

function onDanmakuSwitch(show) {
  hideDanmaku.value = !show;
  show ? danmakuView.show() : danmakuView.hide();
}

let timerId = null;

function onControlMouseMove(){
  
  if(timerId != null){
    onControlMouseLeave();
  }

  controlMouseHover.value = true;

  timerId = setTimeout(()=>{
    controlMouseHover.value = false;
    timerId = null;
  }, 3000);
}

function onControlMouseLeave(){
  clearTimeout(timerId);
  timerId = null;
  controlMouseHover.value = false;
}

</script>

<template>
  <div>
    <div
      ref="playerContainerElement"
      class="w-100 h-100"
      :class="{ 'page-fullscreen': pageFullscreen }"
      :style="{ position: pageFullscreen ? 'fixed' : 'relative' }"
    >
      <!-- <video
        class="w-100 m-auto"
        ref="videoElement"
        :style="pageFullscreen ? videoElementSize : ''"
      ></video> -->
      <FlvPlayer :volume="volume/100.0" :playing="playing" :video_src="video_src"></FlvPlayer>

      <!--弹幕容器-->
      <div
        class="position-absolute m-0"
        style="z-index: 1; left: 0; top: 0%; bottom: 0%; right: 0%;"
        ref ="danmakuContainerElement"
      ></div>

      <!--播放器控制-->
      <div
        class="position-absolute m-0 overflow-hidden"
        style="z-index: 2; left: 0; top: 0%; bottom: 0%; right: 0%;"
        @mousemove="onControlMouseMove"
        @mouseleave="onControlMouseLeave"
      >
        <!--环绕控制区-->
        <div
          class="w-100 h-100 d-flex flex-column justify-content-between"
        >
          <div></div>

          <!--中央-->
          <div class="align-self-center" v-if="display_mode == 'top tv'">
            <button
              class="btn btn-outline-secondary text-light align-self-center"
            >
              进入直播间
            </button>
          </div>
          <!--底部-->
          <div
            class="d-flex justify-content-between player-bottom-controls"
            :class="{'player-bottom-controls-show': controlMouseHover}"
          >
            <!--左下角控制区-->
            <div class="d-flex align-items-center">
              <!--暫停按鈕-->
              <button class="btn" @click="onPause" v-if="playing">
                <svg
                  t="1634957175455"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="2427"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M426.666667 138.666667v746.666666a53.393333 53.393333 0 0 1-53.333334 53.333334H266.666667a53.393333 53.393333 0 0 1-53.333334-53.333334V138.666667a53.393333 53.393333 0 0 1 53.333334-53.333334h106.666666a53.393333 53.393333 0 0 1 53.333334 53.333334z m330.666666-53.333334H650.666667a53.393333 53.393333 0 0 0-53.333334 53.333334v746.666666a53.393333 53.393333 0 0 0 53.333334 53.333334h106.666666a53.393333 53.393333 0 0 0 53.333334-53.333334V138.666667a53.393333 53.393333 0 0 0-53.333334-53.333334z"
                    p-id="2428"
                  ></path>
                </svg>
              </button>

              <!--播放按钮-->
              <button class="btn" @click="onPlay" v-if="!playing">
                <svg
                  t="1635335854612"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="2419"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M870.2 466.333333l-618.666667-373.28a53.333333 53.333333 0 0 0-80.866666 45.666667v746.56a53.206667 53.206667 0 0 0 80.886666 45.666667l618.666667-373.28a53.333333 53.333333 0 0 0 0-91.333334z"
                    p-id="2420"
                  ></path>
                </svg>
              </button>

              <!--刷新按钮-->
              <button class="btn">
                <svg
                  t="1634959851381"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="3539"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M968.604 366.465H762.475c-26.297 0-47.567-21.306-47.567-47.57 0-26.26 21.271-47.567 47.567-47.567h82.748C770.58 165.856 647.837 96.912 508.776 96.912c-227.685 0-412.26 184.576-412.26 412.257 0 227.685 184.576 412.26 412.26 412.26 227.685 0 412.26-184.577 412.26-412.26 0-26.26 21.27-47.567 47.566-47.567 26.263 0 47.57 21.307 47.57 47.567 0 280.226-227.191 507.397-507.396 507.397-280.21 0-507.398-227.17-507.398-507.397 0-280.224 227.189-507.395 507.398-507.395 170.062 0 320.27 83.846 412.259 212.306v-85.457c0-26.263 21.27-47.568 47.566-47.568 26.263 0 47.57 21.305 47.57 47.568v190.273c0.004 26.263-21.307 47.57-47.567 47.57z"
                    p-id="3540"
                  ></path>
                </svg>
              </button>

              <!--音量调节-->
              <VolumeBar :volume="volume" @volumeChange="onVolumeChange"></VolumeBar>

              <!--全屏时启用弹幕输入框-->
              <!-- <DanmakuController
                class="pl-3 dark-theme"
                v-if="fullScreen || pageFullscreen || widescreen"
              ></DanmakuController> -->
            </div>

            <!--右下角控制区-->
            <div class="d-flex flex-row-reverse align-items-center">
              <!--全屏按钮-->
              <button
                class="btn"
                v-if="!fullScreen"
                @click="onFullScreenSwitch(true)"
              >
                <svg
                  t="1635421603466"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="3153"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M983.04 727.04a40.96 40.96 0 0 0-40.96 40.96v173.592381h-174.08a40.96 40.96 0 1 0 0 82.407619h173.592381A82.407619 82.407619 0 0 0 1024 941.592381v-173.592381a40.96 40.96 0 0 0-40.96-40.96zM941.592381 0h-173.592381a40.96 40.96 0 1 0 0 82.407619h173.592381v173.592381a40.96 40.96 0 1 0 82.407619 0V82.407619A82.407619 82.407619 0 0 0 941.592381 0zM256 941.592381H82.407619v-173.592381a40.96 40.96 0 1 0-82.407619 0v173.592381A82.407619 82.407619 0 0 0 82.407619 1024h173.592381a40.96 40.96 0 1 0 0-82.407619zM40.96 296.96a40.96 40.96 0 0 0 40.96-40.96V82.407619h174.08a40.96 40.96 0 1 0 0-82.407619H82.407619A82.407619 82.407619 0 0 0 0 82.407619v173.592381a40.96 40.96 0 0 0 40.96 40.96z"
                    p-id="3154"
                  ></path>
                  <path
                    d="M219.428571 219.428571m82.407619 0l420.32762 0q82.407619 0 82.407619 82.407619l0 420.32762q0 82.407619-82.407619 82.407619l-420.32762 0q-82.407619 0-82.407619-82.407619l0-420.32762q0-82.407619 82.407619-82.407619Z"
                    p-id="3155"
                  ></path>
                </svg>
              </button>

              <!--退出全屏-->
              <button
                class="btn"
                v-if="fullScreen"
                @click="onFullScreenSwitch(false)"
              >
                <svg
                  t="1635575410904"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="2536"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M379.336 697.237L153.362 921.55c-14.11 14.007-36.905 13.922-50.912-0.188-14.007-14.11-13.922-36.905 0.188-50.912l227.6-225.927H138.645c-18.99 0-34.385-15.446-34.385-34.5 0-19.053 15.395-34.5 34.385-34.5H413.72c18.99 0 34.384 15.447 34.384 34.5v276c0 9.15-3.622 17.926-10.07 24.396a34.326 34.326 0 0 1-24.314 10.104 34.326 34.326 0 0 1-24.314-10.104 34.559 34.559 0 0 1-10.071-24.396V697.237z m263.395-366.88l227.813-227.813c14.059-14.059 36.853-14.059 50.912 0 14.059 14.059 14.059 36.853 0 50.912l-225.18 225.18h187.147c18.99 0 34.385 15.445 34.385 34.5 0 19.053-15.395 34.5-34.385 34.5H608.346c-18.99 0-34.384-15.447-34.384-34.5v-276c0-9.15 3.622-17.926 10.07-24.396a34.326 34.326 0 0 1 24.314-10.105c9.12 0 17.865 3.635 24.314 10.105a34.559 34.559 0 0 1 10.07 24.395v193.223zM99.385 410a34.326 34.326 0 0 1-24.314-10.105A34.559 34.559 0 0 1 65 375.5v-276C65 80.446 80.395 65 99.385 65h275.077c18.99 0 34.384 15.446 34.384 34.5 0 19.054-15.394 34.5-34.384 34.5H133.769v241.5c0 9.15-3.622 17.925-10.07 24.395A34.326 34.326 0 0 1 99.384 410z m825.23 552H649.538c-18.99 0-34.384-15.446-34.384-34.5 0-19.054 15.394-34.5 34.384-34.5h240.693V651.5c0-19.054 15.394-34.5 34.384-34.5 18.99 0 34.385 15.446 34.385 34.5v276c0 19.054-15.395 34.5-34.385 34.5z"
                    p-id="2537"
                  ></path>
                </svg>
              </button>

              <!--进入宽屏-->
              <button
                class="btn"
                v-if="!widescreen"
                @click="onSwitchWidescreen(true)"
              >
                <svg
                  t="1635669877743"
                  class="icon fill-light"
                  viewBox="0 0 1280 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="5386"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M1088 0a192 192 0 0 1 191.68 180.736L1280 192v640a192 192 0 0 1-180.736 191.68L1088 1024H192a192 192 0 0 1-191.68-180.736L0 832V192A192 192 0 0 1 180.736 0.32L192 0h896z m0 128H192a64 64 0 0 0-63.552 56.512L128 192v640a64 64 0 0 0 56.512 63.552L192 896h896a64 64 0 0 0 63.552-56.512L1152 832V192a64 64 0 0 0-56.512-63.552L1088 128zM444.224 341.824A12.8 12.8 0 0 1 448 350.912v322.176a12.8 12.8 0 0 1-21.824 9.088L264.96 520.96a12.8 12.8 0 0 1 0-18.048L426.24 341.76a12.8 12.8 0 0 1 18.048 0z m391.552 0a12.8 12.8 0 0 1 14.72-2.368l3.328 2.368L1015.04 503.04a12.8 12.8 0 0 1 2.368 14.72l-2.368 3.328-161.152 161.152a12.8 12.8 0 0 1-21.376-5.696L832 673.088V350.912a12.8 12.8 0 0 1 3.776-9.088z"
                    p-id="5387"
                  ></path>
                </svg>
              </button>

              <!--退出宽屏-->
              <button
                class="btn"
                v-if="widescreen"
                @click="onSwitchWidescreen(false)"
              >
                <svg
                  t="1635670372868"
                  class="icon fill-light"
                  viewBox="0 0 1170 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="5631"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M950.838629 1023.9802H219.424714C99.052065 1023.9802 0 924.928495 0 804.555486V219.424714C0 99.052065 99.051705 0 219.424714 0h731.413915c120.372469 0 219.424714 99.051705 219.424714 219.424714v585.130772c0 120.372649-99.051705 219.424714-219.424714 219.424714zM221.055483 99.665853c-39.458469-1.828944-126.235314 47.798246-119.84076 121.200633l-0.51911 583.689c-2.786526 45.333915 75.619943 124.659085 118.727841 119.080634l729.972143 0.407332c43.883503 6.211859 122.34523-71.850457 117.666402-119.705942l-0.054539-583.852796c5.147179-43.160817-71.739939-123.128215-117.066834-120.058735l-728.884963-0.760306z"
                    p-id="5632"
                  ></path>
                  <path
                    d="M897.444472 713.129872a54.139703 54.139703 0 0 1-36.570786-13.896989l-166.031097-146.282783c-11.926567-10.411717-18.780473-25.492902-18.780473-41.324853s6.853906-30.913316 18.780473-41.325033l166.031097-146.282782a55.197182 55.197182 0 0 1 36.570786-13.85811c30.272529 0 55.182962 24.910433 55.182962 55.182962 0 15.796491-6.782808 30.856437-18.612356 41.325033l-119.951818 105.323503 119.220492 105.323683c11.677632 10.403437 18.365941 25.319745 18.365941 40.95928 0 30.093252-24.762836 54.856089-54.856089 54.856089h-0.080638 0.731506z m-604.879986 0h-0.080639c-30.093072 0-54.856089-24.762836-54.856088-54.856089 0-15.639535 6.688309-30.555843 18.365941-40.95928l119.951997-105.323683-119.951997-105.323503c-11.829549-10.468596-18.612176-25.528541-18.612176-41.325033 0-30.272529 24.910433-55.182962 55.182962-55.182962a55.196462 55.196462 0 0 1 36.570606 13.85811l166.031097 146.282782c11.926747 10.411717 18.780653 25.493082 18.780653 41.325033s-6.853906 30.913136-18.780653 41.324853l-166.031097 146.282783a54.869588 54.869588 0 0 1-36.570606 13.896989z"
                    p-id="5633"
                  ></path>
                </svg>
              </button>

              <!--进入网页全屏-->
              <button
                class="btn"
                v-if="!pageFullscreen"
                @click="pageFullscreen = true"
              >
                <svg
                  t="1635680402410"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="1394"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M908.8 896H115.2c-22.4 0-40-17.6-40-40v-688C75.2 145.6 92.8 128 115.2 128h792c22.4 0 40 17.6 40 40v688c1.6 22.4-16 40-38.4 40zM155.2 816h712V208H155.2v608z"
                    p-id="1395"
                  ></path>
                  <path
                    d="M257.6 524.8c-22.4 0-40-17.6-40-40v-176c0-22.4 17.6-40 40-40h176c22.4 0 40 17.6 40 40s-17.6 40-40 40h-136v136c0 22.4-17.6 40-40 40zM766.4 755.2h-176c-22.4 0-40-17.6-40-40s17.6-40 40-40h136v-136c0-22.4 17.6-40 40-40s40 17.6 40 40v176c0 22.4-17.6 40-40 40z"
                    p-id="1396"
                  ></path>
                </svg>
              </button>
              <!--退出网页全屏-->
              <button
                class="btn"
                v-if="pageFullscreen"
                @click="pageFullscreen = false"
              >
                <svg
                  t="1635680173087"
                  class="icon fill-light"
                  viewBox="0 0 1024 1024"
                  version="1.1"
                  xmlns="http://www.w3.org/2000/svg"
                  p-id="1194"
                  width="1.2em"
                  height="1.2em"
                >
                  <path
                    d="M908.8 896H115.2c-22.4 0-40-17.6-40-40v-688C75.2 145.6 92.8 128 115.2 128h792c22.4 0 40 17.6 40 40v688c1.6 22.4-16 40-38.4 40zM155.2 816h712V208H155.2v608z"
                    p-id="1195"
                  ></path>
                  <path
                    d="M433.6 268.8c22.4 0 40 17.6 40 40v176c0 22.4-17.6 40-40 40h-176c-22.4 0-40-17.6-40-40s17.6-40 40-40h136v-136c0-22.4 17.6-40 40-40zM590.4 499.2h176c22.4 0 40 17.6 40 40s-17.6 40-40 40h-136v136c0 22.4-17.6 40-40 40s-40-17.6-40-40v-176c0-22.4 17.6-40 40-40z"
                    p-id="1196"
                  ></path>
                </svg>
              </button>

              <!--清晰度选择-->
              <div class="btn dropup">
                <button
                  type="button"
                  class="btn text-light btn-sm"
                  data-toggle="dropdown"
                >
                  1080P
                </button>
                <div class="dropdown-menu dropdown-menu-right bg-dark">
                  <button class="dropdown-item text-light small">
                    1080P60
                  </button>
                  <button class="dropdown-item active text-light small">
                    1080P
                  </button>
                  <button class="dropdown-item text-light small">720P</button>
                  <button class="dropdown-item text-light small">480P</button>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.player-controls {
  min-width: 640px;
}

.player-bottom-controls {
  background-image: linear-gradient(#00000000, #00000088, #000000bb, #000000ff);
  visibility: hidden;
  transform:translateY(100px);
  transition:.3s;
}

.player-bottom-controls-show{
  visibility:visible;
  opacity:1;
  transform:translateY(0);
}

.page-fullscreen {
  position: fixed;
  left: 0%;
  bottom: 0%;
  top: 0%;
  right: 0%;
  max-width: 100%;
  min-height: 100%;
  padding: 0%;
  background-color: black;
  margin: auto;
  z-index: 3;
}

</style>
