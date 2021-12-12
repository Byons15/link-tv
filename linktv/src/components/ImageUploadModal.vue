<script setup lang="ts">
</script>

<template>
  <div class="position-fixed w-100 h-100 bg-translucent">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <div class="modal-title">上传你的头像</div>
          <button type="button" class="close">
            <span aria-hidden="true">&times;</span>
          </button>
        </div>
        <div class="modal-body">
          <div class="custom-file">
            <input
              type="file"
              class="custom-file-input"
              id="customFileLangHTML"
              accept="image/png,image/jpg,image/jpeg"
            />
            <label
              class="custom-file-label"
              for="customFileLangHTML"
              data-browse="选择"
              >支持jpg png jpeg格式，最大不超过2MB</label
            >
          </div>
        </div>
        <div class="modal-footer"></div>
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