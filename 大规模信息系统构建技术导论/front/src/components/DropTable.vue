<template>
  <div id="drop">
    <div class="line">
      <div class="tip">DROP TABLE</div>
      <el-select v-model="tableName" placeholder="Select" size="large">
        <el-option
        v-for="(val, index) in removable" :key="index"
        :label="val" :value="val"/>
      </el-select>
    </div>
    <el-button id="btn" type="danger" @click="dropTable">
      删除表
    </el-button>
  </div>
</template>

<script>
import axios from 'axios'
import pubsub from 'pubsub-js'

export default {
    name: 'DropTable',
    computed: {
      removable() {
        // 仅{可写} table 才可被移除
        return this.$store.state.tables.filter(item => item.writable).map(item => item.name)
      }
    },
    data() {
      return {
        tableName: ''
      }
    },
    methods: {
      dropTable() {
        if(this.tableName === "") {
          this.$message({
            message: "请选择 Table",
            type: 'warning'
          });
          return
        }

        this.$confirm(`此操作将永久删除数据表 ${this.tableName} , 是否继续?`, '提示', {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          type: 'warning'
        }).then(() => {

        var tName = this.tableName
        var sql = `DROP TABLE ${tName}`

        // query Master
        axios.get(`/master/write?tableName=${tName}`)
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
                axios.post(`http://${res.data.addr}/drop`,
                  {
                    tableName: tName,
                    sql: sql
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
                        message: '删除成功',
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
        }).catch(() => {
          this.$message({
            type: 'info',
            message: '已取消删除'
          });
          // clear
          this.tableName = ""
        });
      }
    }
}
</script>

<style scoped>
#drop {
  position: relative;
}

.line {
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
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