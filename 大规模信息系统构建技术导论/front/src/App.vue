<template>
  <div id="app">
    <div class="title">分布式数据库 Dash Board</div>
    <div class="body">
        <div class="left-wrap">
            <div class="top">
                <div class="title">Connect Status</div>
                <div class="status" v-if="status">OK</div>
                <div class="status lost" v-else>LOST</div>
            </div>
            <div class="mid">
                <div class="sub">可用 TABLE</div>
                <el-button type="primary" plain @click="handleClick">刷新数据</el-button>
            </div>
            <div class="bottom">
                <el-table :data="$store.state.tables" stripe style="width:!00%">
                    <el-table-column prop="name" label="表名"/>
                    <el-table-column prop="writable" label="状态">
                        <template #default="scope">
                            <el-tag :type="scope.row.writable ? 'success' : 'warning' ">
                                {{scope.row.writable ? '可写' : '只读'}}
                            </el-tag>
                        </template>
                    </el-table-column>
                </el-table>
            </div>
        </div>
        <div class="right-wrap">
            <div class="top">
                <div class="title">Executions</div>
            </div>
            <div class="mid">
                <router-link class="tab" active-class="active" to="/create">
                    Create Table
                </router-link>
                <router-link class="tab" active-class="active"  to="/drop">
                    Drop Table
                </router-link>
                <router-link class="tab" active-class="active"  to="/select">
                    Select
                </router-link>
                <router-link class="tab" active-class="active"  to="/else">
                    Else
                </router-link>
            </div>
            <div class="bottom">
                <router-view/>
            </div>
        </div>
    </div>
  </div>
</template>

<script>
import axios from 'axios'
import { mapMutations } from 'vuex'
import pubsub from 'pubsub-js'

export default {
    name: 'App',
    data() {
        return {
            status: false
        }
    },
    methods: {
        ...mapMutations(['UPDATE']),
        handleClick() {
            pubsub.publish('refresh', "")
        },
        getMeta() {
            console.log("get meta")
            axios.get( '/master/meta')
            .then(
                res => {
                        this.status = true
                        this.UPDATE(res.data.data)
                    },
                err => {
                     this.status = false
                     this.UPDATE([])
                }
            )
        }
    },
    mounted() {
        // 从 Master 处获取 meta 信息
        this.getMeta()
        // 订阅订阅可能涉及 tables 列表变化的信息
        this.pubID = pubsub.subscribe('refresh', () => {
            this.getMeta()
        })
    },
    beforeDestroy() {
        // 取消订阅
        pubsub.unsubscribe(this.pubID)
    }
}

</script>

<style>

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    user-select: none;
    -webkit-user-drag: none;
}

a {
    text-decoration: none;
}

#app {
    width: 100%;
    min-height: 100vh;
    color: #2c3e50;
    padding: 0 20px;
}

#app > .title {
    width: 100%;
    height: 80px;
    line-height: 80px;
    text-align: center;
    color: #5cB87a;
    font-size: 30px;
    font-weight: 700;
    border-bottom: solid 1px #e4e7ed;
}

#app .body {
    width: 100%;
    min-height: calc(100vh - 80px);
    padding: 25px 15px;
    display: flex;
}

#app .body .left-wrap {
    flex: 1;
    border-right: 1px solid #e4e7ed;
}

#app .body .left-wrap .top {
    position: relative;
    width: 100%;
}

#app .body .left-wrap .top .title {
    color: #2c3e50;
    font-size: 20px;
    font-weight: 600;
}

#app .body .left-wrap .top .status {
    position: absolute;
    width: 40px;
    height: 20px;
    border-radius: 4px;
    line-height: 20px;
    text-align: center;
    bottom: 0;
    right: 10px;
    background-color: #5cB87a;
    color: white;
    font-size: 10px;
    font-weight: 500;
}
#app .body .left-wrap .top .status.lost {
    background-color: #e4e7ed;
}

#app .body .left-wrap .mid {
    border-top: 1px solid #e4e7ed;
    margin-top: 10px;
    padding-top: 10px;
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
    justify-content: space-around;
}
#app .body .left-wrap .mid {
    font-size: 15px;
    font-weight: 600;
}

#app .body .right-wrap {
    flex: 3;
    padding: 0 10px;
}

#app .body .right-wrap .top {
    width: 100%;
    height: 40px;
    line-height: 40px;
    border-bottom: 1px solid #e4e7ed;
}
#app .body .right-wrap .top .title {
    color: #2c3e50;
    font-size: 20px;
    font-weight: 600;
}

#app .body .right-wrap .mid {
    padding-top: 10px;
    width: 100%;
    height: 30px;
    display: flex;
    gap: 5px;
    align-items: center;
}

#app .body .right-wrap .mid .tab {
    flex-shrink: 0;
    border-top-left-radius: 8px;
    border-top-right-radius: 8px;
    flex-shrink: 0;
    font-size: 15px;
    width: 120px;
    height: 30px;
    line-height: 30px;
    text-align: center;
    background-color: #e4e7ed;
    color: #606266;
}
#app .body .right-wrap .mid .tab.active {
    background-color: #5cB87a;
    color: white;
}

#app .body .right-wrap .bottom {
    width: 100%;
    padding: 10px;
    padding-top: 20px;
}

</style>
