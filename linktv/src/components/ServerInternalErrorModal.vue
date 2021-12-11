<script setup>
const emites = defineEmits(["closed"]);
</script>

<template>
  <div class="position-fixed w-100 h-100 bg-translucent" v-if="logoutConfirm">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <button type="button" class="close" @click="$emit('closed')">
            <span aria-hidden="true">&times;</span>
          </button>
        </div>
        <div class="modal-body">服务器发生内部错误。</div>
        <div class="modal-footer">
          <button type="button" class="btn btn-primary" @click="$emit('closed')">
            关闭
          </button>
        </div>
      </div>
    </div>
  </div>
</template>