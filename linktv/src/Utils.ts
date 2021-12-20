import {ref} from 'vue';

export interface IUserStore{
    id?: number,
    name?: string,
    email?: string,
    image?: string,
    token?: string,
    logined?: boolean,
    login?:(token: string)=>void,
    logout?: ()=>void,
    update?: ()=>void,
}