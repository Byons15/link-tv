import { createRouter, createWebHashHistory } from 'vue-router';

import Home from './Home.vue'
import Live from './Live.vue'
import Signup from './Signup.vue'

const routes = [
    {
        path: '/live/:id',
        component: Live
    },
    {
        path: '/home',
        component: Home
    },
    {
        path: '/',
        redirect: 'home'
    },
    {
        path: '/signup',
        component: Signup
    }
]

const Router = createRouter({
    history: createWebHashHistory(),
    routes: routes
}
)

export default Router