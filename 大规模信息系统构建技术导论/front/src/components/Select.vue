<template>
  <div id="select">
    <div class="line">
      <div class="tip">操作涉及的 Table</div>
      <el-select v-model="tableName" placeholder="Select" size="large">
        <el-option
        v-for="(val, index) in tables" :key="index"
        :label="val" :value="val"/>
      </el-select>
    </div>
    <div class="tip">
      Input SELECT SQL
    </div>
    <el-input style="margin: 10px 0;"
      v-model="sql" autosize
      type="textarea" placeholder="please input"/>
    <el-button type="primary" @click="selectFrom">执行</el-button>

    <div class="res">
      <div class="tip">Result</div>
      <el-table :data="result.res" style="width: 100%">
        <el-table-column v-for="item in result.meta" :key="item.index"
         :prop="item" :label="item"/>
      </el-table>
    </div>
    
  </div>
</template>

<script>
import axios from 'axios'
import pubsub from "pubsub-js";

export default {
    name: 'SELECT',
    computed: {
      tables() {
        // 仅{可写} table 才可被移除
        return this.$store.state.tables.map(item => item.name)
      }
    },
    data() {
      return {
        sql: '',
        tableName: '',
        result: {
          meta: [],
          res: []
        }
      }
    },
    methods: {
      selectFrom() {
        if(this.tableName === "" || this.sql == "") {
          this.$message({
            message: "选择 table 并填写 sql 语句",
            type: 'warning'
          });
          return
        }

        // find in local cache first
        let cached_addr = localStorage.getItem(this.tables)
        let cache_valid = false
        // if the table name is in the cache
        if (cached_addr) {
          axios.post(`http://${cached_addr}/select`,
              {
                sql: this.sql
              },
              {
                headers: {
                  'Content-Type' : 'application/json'
                }
              }
          ).then(
              res => {
                // the cache is valid
                if (res.data.status == 200) {
                  this.result.meta = res.data.meta
                  this.result.res = res.data.data
                  this.$message({
                    message: '筛选成功',
                    type: 'success'
                  });
                  cache_valid = true
                  this.tableName = ""
                  this.sql = ""
                }
                // the cache is invalid
                else {
                  localStorage.removeItem(this.tableName)
                }
              }
          )
        }

        if (cache_valid)
          return

        let addr
        axios.get(`/master/read?tableName=${this.tableName}`)
        .then(
          res => {
            if(res.data.status == 204) {
              this.$message({
                  message: res.data.msg,
                  type: 'warning'
                });
            } else if (res.data.status == 200) {
              addr = res.data.addr
              axios.post(`http://${res.data.addr}/select`,
                  {
                    sql: this.sql
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
                      this.result.meta = []
                      this.result.res = []
                    } else {
                      this.result.meta = res.data.meta
                      this.result.res = res.data.data
                      this.$message({
                        message: '筛选成功',
                        type: 'success'
                      });
                      // add the mapping to cache
                      localStorage.setItem(this.tableName, addr)
                    }
                  }
                )
                this.tableName = ""
                this.sql = ""
            }
          }
        )
      }
    }
}
</script>

<style scoped>
.tip {
  font-size: 30px;
  font-weight: 600;
}

.res {
  width: 100%;
  margin-top: 20px;
}
.line {
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
}
</style>