import{d as f,r as l,o as u,n as _,a as n,c as r,b as e,k as v,t as m,E as h,I as y,K as x,l as b}from"./vendor.b393d787.js";import{_ as k,a as C}from"./navbar.ea0ba9ca.js";import{_ as $,R as g,L as w}from"./index.800d8c56.js";const A={class:"overflow-hidden"},I={class:"card-body px-2 py-0"},j={class:"card-title mb-1"},M={class:"d-flex justify-content-between"},B={class:"small director"},E=f({props:{liveId:null,playAddress:null,title:null,director:null},setup(t){const a=t,s=l(!0),i=l(0);u(()=>{_(()=>{})});function d(){s.value=!0}function c(){s.value=!1}function o(){g.push("/live/"+a.liveId.toString())}return(p,S)=>(n(),r("div",{class:"card tv-card p-0 border-0",onClick:o},[e("div",A,[e("div",{class:"min-player",onMouseover:d,onMouseleave:c},[v(k,{video_src:t.playAddress,playing:s.value,volume:i.value},null,8,["video_src","playing","volume"])],32)]),e("div",I,[e("div",j,m(t.title),1),e("div",M,[e("div",B,m(t.director),1)])])]))}});var L=$(E,[["__scopeId","data-v-13d4e222"]]);const V={class:"row"},T={key:0,class:"col-12 text-center"},D=f({setup(t){const a=l(),s=l(!1),i=h("errorModal");return u(()=>{_(()=>{new w().all().then(c=>{a.value=c,s.value=a.value.length==0}).catch(()=>{i.value.show()})})}),(d,c)=>(n(),r("div",V,[s.value?(n(),r("div",T,"\u4EC0\u4E48\u90FD\u6CA1\u6709~~~")):(n(!0),r(y,{key:1},x(a.value,(o,p)=>(n(),b(L,{class:"col-3",liveId:o.id,attendance:"2000",playAddress:o.flvAddress,title:o.name,director:o.director,key:p},null,8,["liveId","playAddress","title","director"]))),128))]))}}),F={class:"container"},N=e("div",{class:"row"},[e("br")],-1),R=e("h4",{class:"row"},"\u6B63\u5728\u76F4\u64AD",-1),G={setup(t){return l(!1),l(!0),u(()=>{_(()=>{})}),(a,s)=>(n(),r("div",null,[v(C),e("div",F,[N,R,v(D)])]))}};export{G as default};
