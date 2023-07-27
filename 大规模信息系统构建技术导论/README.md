# README

## 1 环境

- 系统：Windows 10

- ZooKeeper 3.8.0 + JRE 8

- NodeJS v18.16.0

- Oracle OpenJDK version 20

## 2 项目初始化

### 2.1 Zookeeper 安装与配置

1. 将 `apache-zookeeper-3.8.0-bin.tar.gz` 移动至合适的安装路径下，解压。

2. 安装目录下创建 `data` & `log`路径，分别用于存储数据和日志。

3. 复制 `/dependencies` 下的 `zoo.cfg` 至安装目录的 `/conf` 文件夹下

    请根据实际情况修改 `dataDir` 与 `dataLogDir` 配置项为实际绝对路径

4. 将 JRE8 的安装路径设置为系统变量 `JAVA_HOME` 

### 2.2 MySQL 初始化

1. 运行 regionServer 的机器需要在本地运行 `init.sql` 脚本以完成数据库初始化

2. 执行以下命令允许使用 `root` 账号进行远程连接：

    ```sql
    USE mysql;
    UPDATE user SET host='%' WHERE user='root';
    flush privileges;
    ```

> 请关闭防火墙并开放 3306 & 9090 & 9091 以确保项目正常运行

### 2.3 前端项目初始化

1. 进入前端项目根目录 `/front` 执行 `npm install` 以安装必要的依赖

2. 请将 `vue.config.js` 中配置 `target` 为 Master 项目所在机器的实际 IP 地址

## 3 项目运行

1. 打开 `/bin` 目录下的 `zkServer.cmd` 以运行 ZooKeeper 服务

2. 运行 Master 项目，自动初始化 Zookeeper 目录结构

3. 运行 RegionServer 项目

4. 运行 Front 项目，使用图形化界面操作数据库

## 4 项目结构

```text
# 仅展示部分文件
.
├── README.md # 项目运行说明
├── init.sql  # 数据库初始化脚本
│
├── dependencies
│   ├── apache-zookeeper-3.8.0-bin.tar.gz # 使用的 ZooKeeper 安装包
│   ├── jre-8u371-windows-x64.exe         # 使用的 JRE 安装包
│   └── zoo.cfg                           # 供参考的 ZooKeeper 配置文件
│
├── front # Client 项目
│   ├── src
│   │   ├── App.vue    # 根组件
│   │   ├── main.js    # 入口文件
│   │   ├── components # 各路由组件
│   │   │   ├── CreateTable.vue
│   │   │   ├── DropTable.vue
│   │   │   ├── ElseChoice.vue
│   │   │   └── Select.vue
│   │   ├── router     # 路由配置
│   │   │   └── index.js
│   │   └── store      # Vuex 共享数据配置
│   │       └── index.js
│   └── vue.config.js  # 项目配置
│
├── master # Master 项目
│   └── src
│       └── main
│           └── java
│               └── main
│                   ├── MasterApplication.java # 入口类
│                   └── util
│                       ├── RegionMeta.java
│                       └── Zookeeper.java
│
└── regionServer # Region Server 项目
    └── src
        └── main
            └── java
                └── main
                    ├── MasterApplication.java # 入口类
                    └── util
                        ├── Checksum.java
                        ├── Dump.java
                        └── Zookeeper.java
```