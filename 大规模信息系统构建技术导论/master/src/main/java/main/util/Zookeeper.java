package main.util;

import com.alibaba.fastjson2.JSONArray;
import com.alibaba.fastjson2.JSONObject;
import org.apache.curator.RetryPolicy;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.framework.recipes.cache.*;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.apache.zookeeper.data.Stat;

import java.util.ArrayList;
import java.util.List;

public class Zookeeper {

    public Zookeeper(int maxRegion, String zkServerIP) {
        this.maxRegion = maxRegion;
        this.zkServer = zkServerIP + ":2181";
    }

    private int maxRegion;

    private String zkServer;

    private ArrayList<RegionMeta> meta = new ArrayList<>();

    private CuratorFramework client;

    public void connect() {
        System.out.println("Trying to connect Zk Sever @" + zkServer);

        RetryPolicy retryPolicy = new ExponentialBackoffRetry(3000, 1);
        this.client = CuratorFrameworkFactory.newClient(zkServer, 5000, 5000, retryPolicy);
        this.client.start();

        // 初始化持久化节点目录
        this.initZk();

        // 读取创建结果
        Stat stat = new Stat();
        try {
            System.out.println("data @ /test = " + new String(client.getData().storingStatIn(stat).forPath("/test")));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
        System.out.println("if you can see the return value, then you're successfully connected.");

        // INIT
        this.initMeta();
    }

    // 在 zookeeper 中创建基础路径
    public void initZk() {
        System.out.println("initZk");
        try {
            // /test
            createNode("/test", "hello!");
            // regions
            for (int i = 1; i <= maxRegion ; i++) {
                createNode("/region" + i + "/tables");
                createNode("/region" + i + "/slaves");
            }
        } catch (Exception e) {
            System.out.println("初始化 zookeeper 节点结构时出错");
        }
    }

    // 有内容的节点
    public void createNode(String path, String content) throws Exception {
        if(!exist(path)) {
            System.out.println("create " + path);
            client.create().creatingParentsIfNeeded().forPath(path, content.getBytes());
        }
    }
    // 无内容节点
    public void createNode(String path) throws Exception {
        if(!exist(path)) {
            client.create().creatingParentsIfNeeded().forPath(path, "".getBytes());
        }
    }

    public Boolean exist(String path) {
        Boolean isExsit = false;
        try {
            Stat stat = this.client.checkExists().forPath(path);
            isExsit = stat != null;
        } catch (Exception e) {
            System.out.println("获取" + path + "存在情况时出错");
        }
        return isExsit;
    }

    // 获取指定路径下所有节点的[数据]！
    public List<String> getChildsData(String parentPath) {
        // 获取所有子节点路径
        List<String> childsData = new ArrayList<>();
        try {
            List<String> childsPath = this.client.getChildren().forPath(parentPath);
            for (String path : childsPath) {
                String data = new String(this.client.getData().forPath(parentPath + "/" + path));
                childsData.add(data);
            }
        } catch (Exception e) {
            System.out.println("子节点数据获取失败");
        }
        return childsData;
    }

    public List<String> getTables(int regionId) {
        return meta.get(regionId-1).tables;
    }

    // 初始化当前数据集的 meta 信息
    public void initMeta() {
        for (int i = 0; i < this.maxRegion; i++) {
            RegionMeta regionMeta = new RegionMeta();
            ZkListener zkListener = new ZkListener(this.client, "/region" + (i + 1), regionMeta);
            // 监听 /master 节点
            zkListener.listenMaster();
            // 监听 /tables 的所有子节点
            zkListener.listenTables();
            // 监听 /slaves 的所有子节点
            zkListener.listenSlaves();
            meta.add(regionMeta);
        }
    }

    // 判断所有 Region 中是否存在给定的 table
    public Boolean hasTable(String tName) {
        for (int i = 0; i < maxRegion; i++) {
            if (meta.get(i).hasTable(tName)) {
                return true;
            }
        }
        return false;
    }

    // 返回 region 下所有服务器列表信息
    public List<String> getServers(int regionId) {
        String basePath = "/region" + regionId;

        // slaves
        List<String> res = this.getChildsData(basePath + "/slaves");
        // master
        try {
            // master
            res.add(new String(this.client.getData().forPath(basePath + "/master")));
        } catch (Exception e) {}

        return res;
    }

    // 是否存在可写的 Region
    public Boolean hasWritable() {
        for (int i = 0; i < maxRegion; i++) {
            if (meta.get(i).isWritable()) {
                return true;
            }
        }
        return false;
    }

    // 给定 table 所在 Region 是否可写
    public Boolean isWritable(String tName) {
        for (int i = 0; i < maxRegion; i++) {
            if (meta.get(i).hasTable(tName)) {
                return meta.get(i).isWritable();
            }
        }
        return false;
    }



    // 返回用于 create Table 的服务器地址
    public String getCreateServer() {
        String addr = "";
        int min_table = 999;
        for(int i = 0 ; i < maxRegion ; i++) {
            RegionMeta r = meta.get(i);
            if(r.isWritable()) {
                // 可写且 table 数量最少的（一样多返回首个）
                int n_table = r.tables.size();
                if(n_table < min_table) {
                    min_table = n_table;
                    addr = r.master;
                }
            }
        }
        return addr;
    }

    // 返回用于 read Table 的服务器地址
    public String getReadServer(String tName) {
        String addr = "";
        for(int i = 0 ; i < maxRegion ; i++) {
            RegionMeta r = meta.get(i);
            if(r.hasTable(tName)) {
                addr = r.getReadServer();
            }
        }
        return addr;
    }

    // 获取 table 对应 region 的服务器
    public String getWriteServer(String tName) {
        String addr = "";
        for(int i = 0 ; i < maxRegion ; i++) {
            if(meta.get(i).hasTable(tName)) {
                addr = meta.get(i).master;
            }
        }
        return addr;
    }

    // 获取指定 region 的 Master 地址
    public String getRegionMaster(int regionID) {
        return meta.get(regionID-1).master;
    }

    // 查看指定 region 是否有可用 server
    public Boolean isAvailable(int reginID) {
        // 有 master == 至少有一台 server
        if(!meta.get(reginID-1).master.equals("")) {
            return true;
        } else {
            return false;
        }
    }


    // 获取 meta 列表（tableName + writable）
    public JSONArray getMeta() {
        JSONArray res = new JSONArray();
        for(int i = 0 ; i < maxRegion ; i++) {
            RegionMeta r = meta.get(i);
            Boolean writable = r.isWritable();
            for(String tName : r.tables) {
                JSONObject record = new JSONObject();
                record.put("name", tName);
                record.put("writable", writable);
                res.add(record);
            }
        }
        return res;
    }

    // 关闭 zookeeper 会话
    public void disconnect() {
        if(this.client != null) {
            this.client.close();
            this.client = null;
        }
    }
}

class ZkListener {

    private RegionMeta regionMeta;
    private String basePath;

    ZkListener(CuratorFramework client, String basePath, RegionMeta regionMeta) {
        this.client = client;
        this.basePath = basePath;
        this.regionMeta = regionMeta;
    }

    private CuratorFramework client;

    // 对指定的 leaf Node 进行监听
    public void listenMaster() {
        try {
            NodeCache nodeCache = new NodeCache(this.client, basePath + "/master");
            MasterListener masterListener = new MasterListener(nodeCache);
            nodeCache.getListenable().addListener(masterListener);
            nodeCache.start();
        } catch (Exception e) {
            System.out.println("监听 " + basePath + " 的 MASTER 时出错");
        }
    }

    // 监听 slave 的创建与删除事件
    public void listenSlaves() {
        try {
            // 测试监听子节点创建事件
            TreeCache treeCache = new TreeCache(this.client, basePath + "/slaves");
            SlaveListener listener = new SlaveListener();
            // 注册监听
            treeCache.getListenable().addListener(listener);
            // 开启监听
            treeCache.start();
        } catch (Exception e) {
            System.out.println("监听 " + basePath + " 的 SLAVES 时出错");
        }
    }

    public void listenTables() {
        try {
            TreeCache treeCache = new TreeCache(this.client, basePath + "/tables");
            TablesListener listener = new TablesListener();
            treeCache.getListenable().addListener(listener);
            treeCache.start();
        } catch (Exception e) {
            System.out.println("监听 " + basePath + " 的 TABLES 时出错");
        }
    }

    class MasterListener implements NodeCacheListener {

        MasterListener(NodeCache nodeCache) {
            this.nodeCache = nodeCache;
        }

        private NodeCache nodeCache;

        @Override
        public void nodeChanged() {
            try {
                // master 已存在/被创建
                ChildData childData = nodeCache.getCurrentData();
                String masterAddr = new String(childData.getData());
                System.out.println(basePath + "'s MASTER is @" + masterAddr);
                regionMeta.master = masterAddr;

            } catch (Exception e){
                // master 被删除
                System.out.println(basePath + " lost its MASTER");
                regionMeta.master = "";
            }
        }
    }

    class SlaveListener implements TreeCacheListener {

        @Override
        public void childEvent(CuratorFramework curatorFramework, TreeCacheEvent treeCacheEvent) throws Exception {
            if (treeCacheEvent.getType() == TreeCacheEvent.Type.NODE_ADDED
                    &&
                    !treeCacheEvent.getData().getPath().equals(basePath + "/slaves")) {
                // 创建新的 slaveNode（第二个条件是忽略父节点本身的创建事件）
                String connectStr = new String(treeCacheEvent.getData().getData());
                regionMeta.slaves.add(connectStr);
                regionMeta.n_slave = regionMeta.slaves.size();
                System.out.println("new SLAVE for " + basePath + " @" + connectStr + ", " + regionMeta.n_slave + " SLAVES available now" );
            } else if (treeCacheEvent.getType() == TreeCacheEvent.Type.NODE_REMOVED) {
                // slaveNode 被删除（更新可用数量与路由信息列表）
                String connectStr = new String(treeCacheEvent.getData().getData());
                regionMeta.slaves.remove(connectStr);
                regionMeta.n_slave = regionMeta.slaves.size();
                System.out.println("lost SLAVE for " + basePath + " @" + connectStr + ", " + regionMeta.n_slave + " SLAVES available now" );
            }
        }
    }

    class TablesListener implements TreeCacheListener {

        @Override
        public void childEvent(CuratorFramework curatorFramework, TreeCacheEvent treeCacheEvent) throws Exception {
            if (treeCacheEvent.getType() == TreeCacheEvent.Type.NODE_ADDED
                    &&
                    !treeCacheEvent.getData().getPath().equals(basePath + "/tables")) {
                String path[] = new String(treeCacheEvent.getData().getPath()).split("/");
                String tableName = path[3];
                regionMeta.tables.add(tableName);
                System.out.println("new TABLE for " + basePath + " :" + tableName);
            } else if (treeCacheEvent.getType() == TreeCacheEvent.Type.NODE_REMOVED) {
                String path[] = new String(treeCacheEvent.getData().getPath()).split("/");
                String tableName = path[3];
                regionMeta.tables.remove(tableName);
                System.out.println("lost TABLE for " + basePath + " :" + tableName);
            }
        }
    }
}