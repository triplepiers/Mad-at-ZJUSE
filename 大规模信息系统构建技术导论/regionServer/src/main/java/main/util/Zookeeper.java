package main.util;

import org.apache.curator.RetryPolicy;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.framework.recipes.cache.*;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.data.Stat;

import javax.sql.DataSource;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

public class Zookeeper {

    public Zookeeper(DataSource dataSource, String selfAddr, String zkServerIP) {
        this.dataSource = dataSource;
        this.selfAddr = selfAddr.getBytes();
        this.zkServerAddr = zkServerIP + ":2181";
    }

    private Boolean isMaster = false;
    private DataSource dataSource;
    private final String basePath = "/region1"; // 本 regionServer 所在的 regionID
    private final String serverID = "1"; // 本 regionServer 的 ServerID
    private byte[] selfAddr; // 本 regionServer 后端项目监听的地址
    private String zkServerAddr;
    private ZkListener zkListener = null;
    private CuratorFramework client;

    public void connect() {
        System.out.println("Trying to connect Zk Sever @" + zkServerAddr);

        RetryPolicy retryPolicy = new ExponentialBackoffRetry(3000, 1);
        this.client = CuratorFrameworkFactory.newClient(zkServerAddr, 5000, 5000, retryPolicy);
        client.start();

        // 读取创建结果
        try {
            Stat stat = new Stat();
            System.out.println("data @ /test = " + new String(client.getData().storingStatIn(stat).forPath("/test")));
            System.out.println("if you can see the return value, then you're successfully connected.");
        } catch (Exception e) {
            System.out.println(e);
        }

        // INIT
        this.init();
    }

    public void disconnect() {
        if(this.client != null) {
            this.client.close();
            this.client = null;
        }
    }

    // 初始化当前数据集的 meta 信息
    public void init() {

        ZkListener zkListener = new ZkListener();
        this.zkListener = zkListener;

        // 1. 检查是否存在 master 节点
        try {
            if (this.client.checkExists().forPath(basePath +  "/master") == null) {
                this.beMaster(); // 尝试成为主节点
            } else {
                this.beSlave(); // 成为从节点
            }
        } catch (Exception e) {
            System.out.println("检查 MASTER 是否存在时出错");
        }

    }

    // 尝试成为 master
    public void beMaster() {
        // 注册临时节点
        try {
            this.client.create().withMode(CreateMode.EPHEMERAL).forPath(basePath + "/master", this.selfAddr);
            this.isMaster = true;
        } catch (Exception e) {
            System.out.println("未能成为 MASTER");
            // be slave
            this.beSlave();
            return;
        }

        System.out.println("current server is a MASTER");

        // 关闭对 /master 的监听
        this.zkListener.stopListenMaster();
        // 删除对应的 slave 节点
        try {
            this.client.delete().forPath(basePath + "/slaves/" + serverID);
        } catch (Exception e) {}

        // 向 /tables 路径写入所有 table 信息
        try {
            Connection conn = dataSource.getConnection();
            Statement stmt = conn.createStatement();
            String sql = "show tables";
            ResultSet rs = stmt.executeQuery(sql);
            // 打印结果
            System.out.println("TABLES in current database are as follows:");
            int countTable = 0;
            while(rs.next()) {
                countTable += 1;
                String tName = rs.getString(1);
                System.out.println(countTable + " " + tName);
                // 写进 /tables
                this.client.create().withMode(CreateMode.EPHEMERAL).forPath(basePath + "/tables/" + tName);
            }
            // 释放连接
            rs.close();
            stmt.close();
            conn.close();
        } catch (Exception e) {
            System.out.println("获取 TABLE 列表失败");
            System.out.println(e);
        }
    }
    // 成为 slave
    public void beSlave() {
        System.out.println("current server is a SLAVE");
        // 把自己的 ID 写到 /slaves 下
        try {
            this.client.create().withMode(CreateMode.EPHEMERAL).forPath(basePath + "/slaves/" + serverID, selfAddr);
            this.isMaster = false;
            // dump
            String masterAddr[] = new String(this.client.getData().forPath(basePath + "/master")).split(":");
            String masterIP = masterAddr[0];
            System.out.println(masterIP);
            Dump.dumpRemoteDB(masterIP);
        } catch (Exception e) {}
        // 注册对 /master 的监听
        this.zkListener.listenMaster();
    }

    // 向 /tables 下添加新的 table 信息
    public void addTable(String tName) {
        try {
            this.client.create().withMode(CreateMode.EPHEMERAL).forPath(basePath + "/tables/" + tName);
        } catch (Exception e) {
            System.out.println("向 zookeeper 添加 TABLE " + tName + " 信息时出错");
        }
    }

    // 从 /tables 中移除 table 信息
    public void removeTable(String tName) {
        try {
            this.client.delete().forPath(basePath + "/tables/" + tName);
        } catch (Exception e) {
            System.out.println("从 zookeeper 移除 TABLE " + tName + " 信息时出错");
        }
    }

    // 判断当 server 是否为 Master 节点
    public Boolean checkMaster() {
        return this.isMaster;
    }

    // 获取所有的 slave 地址
    public List<String> getSlaves() {
        // 获取所有子节点路径
        List<String> slavePaths = new ArrayList<>();
        try {
            List<String> childPaths = this.client.getChildren().forPath(basePath + "/slaves");
            for (String path : childPaths) {
                String data = new String(this.client.getData().forPath(basePath + "/slaves/" + path));
                slavePaths.add(data);
            }
        } catch (Exception e) {
            System.out.println("slave地址获取失败");
            System.out.println(e);
        }
        return slavePaths;
    }

    public void checkTable(String tName) {
        Checksum c = new Checksum(dataSource);
        try {
            this.client.setData().forPath(basePath + "/tables/" + tName, c.getCRC4table(tName).toString().getBytes());
        } catch (Exception e) {
            System.out.println("更新 zookeeper 中 TABLE " + tName + " 校验和出错");
        }
    }

    class ZkListener {

        public void stopListenMaster() {
            if(this.treeCache != null) {
                try {
                    this.treeCache.close();
                } catch (Exception e) {
                    System.out.println("未能成功停止对 master 的监视");
                }
            }
        }

        private TreeCache treeCache = null;


        // 对指定的 leaf Node 进行监听
        public void listenMaster() {
            try {
                TreeCache treeCache = new TreeCache(client, basePath + "/master");
                this.treeCache  = treeCache ;
                MasterListener masterListener = new MasterListener();
                treeCache .getListenable().addListener(masterListener);
                treeCache .start();
            } catch (Exception e) {
                System.out.println("监听 " + basePath + " 的 MASTER 时出错");
            }
        }

        class MasterListener implements TreeCacheListener {

            @Override
            public void childEvent(CuratorFramework curatorFramework, TreeCacheEvent treeCacheEvent) throws Exception {
                if(treeCacheEvent.getType() == TreeCacheEvent.Type.NODE_REMOVED) {
                    // master 被删除
                    System.out.println("原 MASTER 失去连接。尝试成为 MASTER ...");
                    beMaster();
                }
            }
        }

    }
}

