# README

![banner](.\img\banner.png)

<p align="center">
    <img src=https://img.shields.io/badge/lisence-MIT-3B9DDF?style=flat-square>
    <img src=https://img.shields.io/badge/VueCli-5.0.0-3EAF7C?style=flat-square>
    <img src=https://img.shields.io/badge/Echarts-5.4.0-E43961?style=flat-square>
    <img src=https://img.shields.io/badge/axios-1.2.0-5A29E4?style=flat-square>
</br>
    <img src=https://img.shields.io/badge/numpy-1.24.0-013243?style=flat-square>
    <img src=https://img.shields.io/badge/sklearn-1.2.0-EC9C4B?style=flat-square>
</br>
    <img src=https://img.shields.io/badge/Python-3.11.0-1E415E?style=flat-square>
    <img src=https://img.shields.io/badge/Flask-2.2.2-000000?style=flat-square>
    <img src=https://img.shields.io/badge/PyMySQL-1.0.2-006DAD?style=flat-square>
    <img src=https://img.shields.io/badge/SQLAlchemy-1.4.45-778877?style=flat-square>
</p>



---

<center>界面预览</center>

![main](.\img\main.png)

​	“渔我所欲”渔业数据可视化平台旨在降低渔业从业人员获取并分析历年行业数据的门槛，提供了一站式数据检索及可视化的开源解决方案。该平台聚合了来自多个官方平台的统计数据，并支持按年份、地区、数据类别等信息进行筛选，以 "数据表 + 产业地图" 的形式直观的向用户展现行业数据的地域差异与时间差异。此外，本平台也实现了基于历年数据的“智能预测”功能，能够预测并以地图形式呈现下一年的预期数据，以帮助用户进行决策。

​	本平台无需登录即可使用，相关数据经爬取处理后存储在服务器数据库中，使用体验流畅；管理员经注册登录后可通过后台系统对数据源信息进行管理。

## 1 目录

```
.
├── UI // 原生三件套撰写的前端界面文件
├── backend // 后端项目
│   ├── app.py   // 启动文件
│   ├── init.py  // 数据库初始化脚本
│   ├── data     // 数据源文件
│   ├── src      // 相关实现代码
│   └── venv     // 后端项目虚拟环境
├── frontend      // 前端项目
│   ├── README.md
│   ├── babel.config.js
│   ├── jsconfig.json
│   ├── package.json
│   ├── public
│   ├── src
│   └── vue.config.js
└── tmp
```

## 2 功能

```
管理员
 |_ 登录
 |_ 注册
 |_ 修改数据源
    |_ 新建数据表
    |_ 对数据进行添加 / 删除 / 修改
    
    
普通用户（无需登录）
  |_ 查看所有数据表
  |_ 筛选数据
  |_ 可视化数据
  |  |_ 产业地图
  |  |_ 饼状图
  |  |_ 柱状图
  |_ 以图片格式导出可视化结果
```

## 3 安装

> 在使用本项目前，您需要确保本地已经安装了 [Git](https://git-scm.com/),  [Node](http://nodejs.org/),  [Vue Cli](https://cli.vuejs.org/zh/guide/) 与 [Python](https://www.python.org/)。

### 克隆本项目

```bash
# 在终端运行以下命令以克隆本项目
git https://github.com/Pikachu-Yuhang/SEM.git

# 通过以下命令进入项目根目录
cd coding
```

### 前端项目配置运行

```bash
# 进入前端项目根目录
cd frontend 

# 检查是否存在 node_modules文件夹 与 package-lock.json文件
# 若存在，请删除

# 安装依赖，出现 All packages installed 表示安装成功
npm install
# 若速度较慢，可以尝试以下命令
npm install --registry=https://registry.npm.taobao.org

# 执行以下命令以运行项目
npm run serve

# 若运行成功，您可以通过以下地址访问前端界面
http://localhost:8080/
```

### 后端项目配置运行

#### 项目配置

1. 连接数据库的准备

   对 `src/database.py` 中的 `DB_URI = 'mysql+pymysql://root:662258@localhost:3306/test'` 做如下修改：

   - 662258 替换为本机 root 账号的密码（不想用 root 也可以把冒号前的用户名改了）
   - 若创建的数据库名称不叫 "test"，那么就对末尾斜杠后的字符串做相应修改

   - 同样的，您需要对 `init.py` 中的 `user` & `password` 做必要修改

2. 自动建表

- 在 `(venv)` 终端中执行 `python init.py` ，以便初始化您的本地数据库

- 若您的数据库中原本不存在名为 `test` 的 database，请注释 `init.py` 中的：

  ```
  sql = "DROP database test;
  db.execute(sql)
  ```

- 终端中显示 `database initialization finished...` 意味着初始化被顺利执行了

> 部分用户在执行自动建表的过程中可能出现问题，此时您可以尝试进行以下操作：
>
> 1. 在 MySQL 中创建名为 `test` 的数据库
> 2. 取消 `init.py` 文件 line 8: `db='test'` 的注释
> 3. 注释 `init.py` 文件 line 15～22
> 4. 重新执行本文件

#### 项目运行

```bash
# 进入后端项目根目录
cd backend

# 打开虚拟环境 -> 成功后命令行首应出现 (venv) 字样
cd venv/Scripts
activate # 若失败，可尝试使用 ./activate

# 在后端项目根目录下执行以下命令
python app.py

# 若运行成功，您可以通过以下地址访问后端项目
http://localhost:5050/
```

## 4 支持

​	“开源项目分析平台”支持现代浏览器及 10.0 以上版本的 IE 浏览器。


| <img src="https://raw.githubusercontent.com/alrra/browser-logos/master/src/edge/edge_48x48.png" alt="IE / Edge" width="24px" height="24px"><br/>IE / Edge | <img src="https://raw.githubusercontent.com/alrra/browser-logos/master/src/firefox/firefox_48x48.png" alt="Firefox" width="24px" height="24px"><br/>Firefox | <img src="https://raw.githubusercontent.com/alrra/browser-logos/master/src/chrome/chrome_48x48.png" alt="Chrome" width="24px" height="24px"><br/>Chrome | <img src="https://raw.githubusercontent.com/alrra/browser-logos/master/src/safari/safari_48x48.png" alt="Safari" width="24px" height="24px"><br/>Safari |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
|                      IE10,  IE11,  Edge                      |                        最新的2个版本                         |                        最新的2个版本                         |                        最新的2个版本                         |

---

<center>Copyright (c) 2022-present SEM-G06</center>



