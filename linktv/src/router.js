import { createRouter, createWebHashHistory } from 'vue-router';

const routes = [
    {
        path: '/live/:id(\\d+)',
        component: ()=> import("./Live.vue")
    },
    {
        path: '/home',
        component: ()=> import('./Home.vue')
    },
    {
        path: '/',
        redirect: 'home'
    },
    {
        path: '/signup',
        component: ()=> import("./Signup.vue")
    }
]

const Router = createRouter({
    history: createWebHashHistory(),
    routes: routes
}
)

export default Router