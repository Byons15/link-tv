# Link-TV

#### 介绍
一套包含了推流端，服务端，web前端的适用于游戏直播的解决方案。目前处于项目初期。  
Linkme（暂不可用）：一个专门运行于windows下的推流软件。核心部件使用c++编写，UI及上层部分由WPF编写。   
LinkServer：为直播系统提供webapi服务，使用ASP.NET 5进行开发。  
linktv：web前端部分，目前仅适应PC端的chrome系列浏览器。  使用VUE3+BootStrap4+vite。  
这个项目是作者《以实践驱动学习》的一个平台，会一直更新。
#### 软件架构
  这套方案视频流的推流使用RTMP协议，播放协议使用HTTP FLV。目前使用腾讯云直播作为视频流服务支撑，当然可以使用Nginx作为流服务器。推流端请使用OBS进行推流，目前Linkme由于还没有与服务器进行对接，所以无法使用。  
  服务端使用MySQL存储，Redis作为缓存，由于项目初期阶段，在数据结构上及其简单。目前仅支持简单的用户登录、注册等功能。  
  前端是作者第一次使用VUE3+BootStrap4+vite，所以自认相当粗糙。

#### 安装教程

1.  xxxx
2.  xxxx
3.  xxxx

#### 使用说明

1.  xxxx
2.  xxxx
3.  xxxx

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
