<script setup lang="ts">
import { ref } from "@vue/reactivity";
import { nextTick, onMounted } from "@vue/runtime-core";
import Cropper from "cropperjs";
import "cropperjs/dist/cropper.css";

const emits = defineEmits(["closed"]);

const fileLabel = ref("支持jpg png jpeg格式。");

const fileInputElement = ref<HTMLInputElement>();

const canvasElement = ref<HTMLCanvasElement>();
let canvasCtx: CanvasRenderingContext2D;
const canvasHeight = ref(270);
const canvasWidth = ref(480);

const srcImgElement = ref<HTMLImageElement>();

let cropper: Cropper;

function cropperEvent(e: Cropper.CropEvent<HTMLImageElement>) {
  let base64Img = cropper
    .getCroppedCanvas({
      imageSmoothingQuality: "high",
    })
    .toDataURL("image/jpg");
    // 显示base64Img内容
}

function onFileInputChanged() {
  fileLabel.value = fileInputElement.value.files[0].name;
  var reader = new FileReader();
  reader.onload = (e) => {
    srcImgElement.value.src = e.target.result as string;
    nextTick(() => {
      cropper = new Cropper(srcImgElement.value, {
        viewMode: 1,
        dragMode: "none",
        initialAspectRatio: 1,
        aspectRatio: 1,
        preview: ".before",
        background: false,
        autoCropArea: 0.6,
        zoomOnWheel: true,
        crop: cropperEvent,
      });
    });
  };

  reader.readAsDataURL(fileInputElement.value.files[0]);
}

onMounted(() => {
  nextTick(() => {});
});
</script>

<template>
  <div class="position-fixed w-100 h-100 bg-translucent">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <div class="modal-title">上传你的头像</div>
          <button type="button" class="close" @click="$emit('closed')">
            <span aria-hidden="true">&times;</span>
          </button>
        </div>
        <div class="modal-body d-flex flex-column justify-content-center">
          <div class="custom-file">
            <input
              type="file"
              class="custom-file-input"
              id="customFileLangHTML"
              accept="image/*"
              ref="fileInputElement"
              @change="onFileInputChanged"
            />
            <label
              class="custom-file-label"
              for="customFileLangHTML"
              data-browse="选择"
              >{{ fileLabel }}</label
            >
          </div>
          <div class="image-container mt-2">
            <img ref="srcImgElement" class="image" />
          </div>
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-primary">上传</button>
          <button
            type="button"
            class="btn btn-secondary"
            @click="$emit('closed')"
          >
            取消
          </button>
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

.image-container {
  overflow: hidden;
}

.image {
  max-width: 100%;
}
</style>