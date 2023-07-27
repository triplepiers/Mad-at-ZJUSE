<template>
  <div id="create">
    <div class="line">
      <div class="tip">CREATE TABLE </div>
      <el-input v-model="tableName" placeholder="Table Name" 
       style="width: 200px;"/>
      <div class="tip"> (</div>
    </div>
    <div class="line">
      <el-input style="margin: 10px 0;"
      v-model="info" autosize
      type="textarea" placeholder="Columns Info"/>
      <div class="tip"> )</div>
    </div>
    <div class="line">
      <el-input style="margin: 10px 0;"
      v-model="option" autosize
      type="textarea" placeholder="Options"/>
      <div class="tip"> ;</div>
    </div>
    <el-button id="btn" type="primary" @click="executeSQL">
      执行
    </el-button>
  </div>
</template>

<script>
import axios from 'axios'
import pubsub from 'pubsub-js'

export default {
    name: 'CreateTable',
    data() {
      return {
        tableName: '',
        info: '',
        option: 'ENGINE=INNODB DEFAULT CHARSET=utf8'
      }
    },
    methods: {
      executeSQL() {
        if(this.tableName === "" || this.info === "") {
          this.$message({
            message: "请填写表名和字段信息",
            type: 'warning'
          });
          return
        }

        // var sql = `CREATE TABLE '${this.tableName}' (${this.info.replaceAll(/[\r\n]/g, ' ')}) ${this.option}`
        var sql = `CREATE TABLE \`${this.tableName}\` (${this.info.replaceAll(/[\r\n]/g, ' ')})`
        var tName = this.tableName

        // query Master
        axios.get(`/master/create?tableName=${tName}`)
        // axios.get(`http://localhost:9090/create?tableName=test`)
        .then(
          res => {
            if(res.data.status == 204) {
              this.$message({
                message: res.data.msg,
                type: 'warning'
              });
            } else if(res.data.status == 200) {
              console.log("route to " + res.data.addr)
              //向指定的 server 发送数据
              axios.post(`http://${res.data.addr}/new`,
                {
                  tableName: tName,
                  sql: sql
                  // tableName: "test",
                  // sql: "CREATE TABLE `test` (`idtest` INT NOT NULL,PRIMARY KEY (`idtest`))"
                },
                {
                  headers: {
                    'Content-Type' : 'application/json'
                  }
                }
              ).then(
                res => {
                  if(res.data.status == 204) {
                    this.$message.error(res.data.msg)
                  } else {
                    this.$message({
                      message: '创建成功',
                      type: 'success'
                    });
                    // 刷新 meta
                    pubsub.publish('refresh', "")
                  }
                }
              )
            }
          }
        )

        // clear
        this.tableName = ""
        this.info = ""
        this.option = "ENGINE=INNODB DEFAULT CHARSET=utf8"
      }
    }
}
</script>

<style scoped>
#create {
  position: relative;
}

.line {
  width: 100%;
  display: flex;
  align-items: center;
  gap: 30px;
}
.line .tip {
  font-size: 30px;
  font-weight: 600;
}

#btn {
  position: absolute;
  margin-top: 10px;
  right: 5px;
}
</style>