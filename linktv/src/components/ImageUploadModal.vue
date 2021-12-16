<script setup lang="ts">
import { ref } from "@vue/reactivity";
import { nextTick, onMounted } from "@vue/runtime-core";
import Cropper from "cropperjs";
import "cropperjs/dist/cropper.css";
import {
  FileParameter,
  IInternalErrorDescription,
  IInvalidModelDescription,
  UserClient,
} from "../LinkClient";
import * as Utils from "../Utils";

const emits = defineEmits(["cancelEvent", "uploadedEvent"]);

const fileLabel = ref("导入图片以进行编辑。");

const fileInputElement = ref<HTMLInputElement>();

const canvasElement = ref<HTMLCanvasElement>();
let canvasCtx: CanvasRenderingContext2D;
const canvasHeight = ref(270);
const canvasWidth = ref(480);

const srcImgElement = ref<HTMLImageElement>();

let cropper: Cropper;

const imgDemo = ref<HTMLCanvasElement>();
let imgDemoCtx: CanvasRenderingContext2D;
let imgDemoSrc: HTMLImageElement;

function cropperEvent(e: Cropper.CropEvent<HTMLImageElement>) {
  nextTick(() => {
    imgDemoCtx.drawImage(
      imgDemoSrc,
      e.detail.x,
      e.detail.y,
      e.detail.width,
      e.detail.height,
      0,
      0,
      64,
      64
    );
  });
}

const loadedAnyFile = ref(false);

function onFileInputChanged() {
  if (fileInputElement.value.files.length == 0) {
    loadedAnyFile.value = false;
    return;
  }

  loadedAnyFile.value = true;
  fileLabel.value = fileInputElement.value.files[0].name;

  var reader = new FileReader();
  reader.onload = (e) => {
    srcImgElement.value.src = e.target.result as string;
    imgDemoSrc = new Image();
    imgDemoSrc.style.display = "none";
    imgDemoSrc.src = e.target.result as string;
    imgDemo.value.appendChild(imgDemoSrc);

    if (cropper != null) {
      cropper.destroy();
      cropper = null;
    }

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
      imgDemoCtx = imgDemo.value.getContext("2d");
    });
  };

  reader.readAsDataURL(fileInputElement.value.files[0]);
}

function onUpload() {
  if (cropper === undefined) return;

  cropper
    .getCroppedCanvas({
      imageSmoothingQuality: "high",
    })
    .toBlob((blob) => {
      var types = blob.type.split("/");
      const api = new UserClient();
      api
        .updateUserImage(0, types[1], <FileParameter>{
          data: blob,
          fileName: fileInputElement.value.files[0].name,
        })
        .catch(() => {
          Utils.errorModal.value.show();
        })
        .then(() => {
          emits("uploadedEvent");
        });
    });
}

onMounted(() => {
  nextTick(() => {});
});
</script>

<template>
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <div class="modal-title">上传你的头像</div>
        <button type="button" class="close" @click="$emit('cancelEvent')">
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
        <canvas
          class="rounded-circle mt-3"
          width="64"
          height="64"
          ref="imgDemo"
          v-if="loadedAnyFile"
          style="width: 64px; height: 64px"
        ></canvas>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-primary" @click="onUpload">
          上传
        </button>
        <button
          type="button"
          class="btn btn-secondary"
          @click="$emit('cancelEvent')"
        >
          取消
        </button>
      </div>
    </div>
  </div>
</template>

<style scoped>

.image-container {
  overflow: hidden;
}

.image {
  max-width: 100%;
}
</style>