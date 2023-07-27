<template>
  <div id="else">
    <div class="line">
      <div class="tip">操作涉及的 Table</div>
      <el-select v-model="tableName" placeholder="Select" size="large">
        <el-option
        v-for="(val, index) in writable" :key="index"
        :label="val" :value="val"/>
      </el-select>
    </div>
    <div class="tip">
      Input SQL
    </div>
    <el-input style="margin: 10px 0;"
      v-model="sql" autosize
      type="textarea" placeholder="please input"/>
    <el-button type="primary" @click="executeSQL">执行</el-button>
  </div>
</template>

<script>
import axios from 'axios'

export default {
    name: 'ElseChoice',
    data() {
      return {
        tableName: '',
        sql: ''
      }
    },
    computed: {
      writable() {
        // 仅{可写} table 才可被移除
        return this.$store.state.tables.filter(item => item.writable).map(item => item.name)
      }
    },
    methods: {
      executeSQL() {
        if(this.tableName === "" || this.sql === "") {
          this.$message({
            message: "请选择 Table 并填写 SQL",
            type: 'warning'
          });
          return
        }

        // query Master
        axios.get(`/master/write?tableName=${this.tableName}`)
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
                axios.post(`http://${res.data.addr}/execute`,
                  {
                    sql: this.sql,
                    tableName: this.tableName
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
                        message: '操作成功',
                        type: 'success'
                      });
                    }
                    // clear
                    this.sql = ""
                    this.tableName = ""
                  }
                )
              }
              // clear
              this.sql = ""
              this.tableName = ""
            }
          )
        }
      }
    
}
</script>

<style>
.tip {
  font-size: 30px;
  font-weight: 600;
}
.line {
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
}
</style>