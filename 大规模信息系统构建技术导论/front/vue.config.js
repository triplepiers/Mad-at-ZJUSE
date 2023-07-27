const { defineConfig } = require('@vue/cli-service')
module.exports = defineConfig({
  transpileDependencies: true,
  lintOnSave: false,
  devServer: {
    port: 8081,
    proxy: {
      "/master": {
        // 请在此处配置 master 的地址
        target: 'http://localhost:9090',
        ws: true,
        changeOrigin: true,
        pathRewrite: {
          '^/master': ''
        }
      }
    }
  }
})
