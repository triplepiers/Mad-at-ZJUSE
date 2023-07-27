import { createApp } from 'vue'
// 使用 Element-UI
import ELementPlus from 'element-plus'
import 'element-plus/dist/index.css'
// 使用 Vue-Router
import router from './router/index.js'
// 使用 Vuex
import store from './store/index.js'

import App from './App.vue'

const app = createApp(App)

app.use(ELementPlus)
app.use(router)
app.use(store)

app.mount("#app")
