// Vue-Router
import {createRouter, createWebHistory } from 'vue-router'
// import Create from ''

const routes = [
    {
        path: '/',
        redirect: '/create'
    },
    {
        path: '/create',
        component: () => import('../components/CreateTable.vue')
    },
    {
        path: '/drop',
        component: () => import('../components/DropTable.vue')
    },
    {
        path: '/select',
        component: () => import('../components/Select.vue')
    },
    {
        path: '/else',
        component: () => import('../components/ElseChoice.vue')
    }
]

const router = createRouter({
    history: createWebHistory(),
    routes
})

export default router