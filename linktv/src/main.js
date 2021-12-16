import "jquery";
import "bootstrap/dist/css/bootstrap.min.css";
import "popper.js";
import "bootstrap";
import { createApp } from "vue"
import App from './App.vue'
import "./linktv.css"
import Router from './Router'
import axios from "axios";


const app = createApp(App)
app.use(Router)
app.mount("#app")
