package main;

import com.alibaba.fastjson2.JSONObject;
import jakarta.annotation.PostConstruct;
import jakarta.annotation.PreDestroy;
import main.util.Zookeeper;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestTemplate;

import java.net.InetAddress;
import java.util.ArrayList;
import java.util.List;

@RestController
@CrossOrigin
@EnableScheduling
@SpringBootApplication(exclude = DataSourceAutoConfiguration.class)
public class MasterApplication {

    // 用来获取配置文件中的端口号
    @Value("${server.port}")
    private int port;
    // 判定 hot point 的 threshold
    private int threshold = 1;
    private static int maxRegion = 2;
    private static Zookeeper zk;

    public static void main(String[] args) {
        SpringApplication.run(MasterApplication.class, args);
    }

    @PostConstruct
    public void init() {
        // 输出本机信息
        try {
            String IP = InetAddress.getLocalHost().getHostAddress();
            System.out.println("Master & Zookeeper are @"+ IP);
            System.out.println("Master listening Port: " + this.port);
            // Zookeeper 连接测试
            Zookeeper zk = new Zookeeper(MasterApplication.maxRegion, IP);
            MasterApplication.zk = zk;
            zk.connect();
        } catch (Exception e) {
            System.out.println("IP信息获取失败");
        }

    }

    @PreDestroy
    public void disconnect() {
        zk.disconnect();
        System.out.println("Zookeeper connection CLOSED");
    }

    @Scheduled(fixedRate = 10000) // 每 10s check 一次
    public void checkHotPoint() {
        System.out.println("Checking HOT POINT...");
        // 对每个 region 执行 count 操作
        List<Integer> countVists = new ArrayList<>();
        int minn = 999;
        int minnIdx = -1;
        int maxx = 0;
        int maxxIdx = -1;

        for( int i = 1 ; i <= maxRegion ; i++) {
            List<String> serverAddr = zk.getServers(i);
            int total = 0;
            for(String server : serverAddr) {
                // 发送 GET 请求
                String url = "http://" + server + "/count";
                // 发起请求
                RestTemplate client = new RestTemplate();
                ResponseEntity<Integer> responseEntity = client.getForEntity(url, Integer.class);
                total += responseEntity.getBody();
            }
            countVists.add(total);

            if (total < minn && zk.isAvailable(i)) {
                minn = total;
                minnIdx = i;
            }
            if (total > maxx) {
                maxx = total;
                maxxIdx = i;
            }
            System.out.println("region" + i + " - " + total + " times");
        }

        // maxx 切一半到 minn
        if (maxxIdx > -1 && minnIdx > -1 && maxx > threshold && maxx > 2*minn) {
            System.out.println("HOST POINT is region[" + maxxIdx + "]，对以下 table 进行迁移");
            List<String> tbList = zk.getTables(maxxIdx);
            for (int i = 0 ; i < (tbList.size()+1)/2 ; i++) {
                System.out.print(" " + tbList.get(i));
                // 迁移
                letDump(maxxIdx, minnIdx, tbList.get(i));
                // drop
                letDrop(maxxIdx,tbList.get(i));
            }
            System.out.print("\n");
        }
    }

    // 将 table 从 region Src 迁移到 region Dest
    private void letDump(int srcRegionID, int destRegionID, String tName) {
        System.out.println("MOVE [" + tName + "] from region[" + srcRegionID + "] to region[" + destRegionID + "]");
        // 获取 srcRegion 的 MasterIP
        String srcIP = new String(zk.getRegionMaster(srcRegionID)).split(":")[0];

        // 获取 destRegion 的 Server 列表（包括 Master）
        List<String> destAddrs = zk.getServers(destRegionID);
        for(String destAddr : destAddrs) {
            // 让每一个 dest 迁移指定 table
            String url = "http://" + destAddr + "/dump";
            // 构建参数
            JSONObject params = new JSONObject();
            params.put("srcIP", srcIP);
            params.put("tableName", tName);

            // 构建 header
            HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.APPLICATION_JSON);

            HttpEntity<JSONObject> httpEntity = new HttpEntity<>(params, headers);
            // 发起请求
            RestTemplate client = new RestTemplate();
            client.postForEntity(url, httpEntity, JSONObject.class);
        }
    }

    // 让 region Src 删除指定 table
    private void letDrop(int regionID, String tName) {
        System.out.println("DELETE [" + tName + "] in region[" + regionID + "]");
        // 获取 region 中的服务器列表
        List<String> destAddrs = zk.getServers(regionID);
        for(String destAddr : destAddrs) {
            // 让每一个 dest 迁移指定 table
            String url = "http://" + destAddr + "/drop";
            // 构建参数
            JSONObject params = new JSONObject();
            params.put("tableName", tName);
            params.put("sql", "DROP TABLE " + tName);

            // 构建 header
            HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.APPLICATION_JSON);

            HttpEntity<JSONObject> httpEntity = new HttpEntity<>(params, headers);
            // 发起请求
            RestTemplate client = new RestTemplate();
            client.postForEntity(url, httpEntity, JSONObject.class);
        }
    }

    @RestController
    @CrossOrigin
    class ClientController {
        // 建表操作
        @RequestMapping("/create")
        public JSONObject createTable(@RequestParam("tableName") String tName) {
            JSONObject res = new JSONObject();
            // 1 检查是否已经存在同名 table
            if(zk.hasTable(tName)) {
                res.put("status", 204);
                res.put("msg", "表" + tName + "已存在");
            } else if(!zk.hasWritable()){
                res.put("status", 204);
                res.put("msg", "暂无可写 Region");
            } else {
                res.put("status", 200);
                res.put("addr", zk.getCreateServer());
            }
            return res;
        }

        // 读取操作
        @RequestMapping("/read")
        public JSONObject readTable(@RequestParam("tableName") String tName) {
            JSONObject res = new JSONObject();
            // 1 检查是否存在 table
            if(!zk.hasTable(tName)) {
                res.put("status", 204);
                res.put("msg", "表" + tName + "不存在");
            } else {
                res.put("status", 200);
                res.put("addr", zk.getReadServer(tName));
            }
            return res;
        }

        // 插入 / 建索引
        @RequestMapping("/write")
        public JSONObject writeTable(@RequestParam("tableName") String tName) {
            JSONObject res = new JSONObject();
            // 1 检查是否存在 table
            if(!zk.hasTable(tName)) {
                res.put("status", 204);
                res.put("msg", "表" + tName + "不存在");
            } else if(!zk.isWritable(tName)){
                res.put("status", 204);
                res.put("msg", "所在区间不可写");
            } else {
                res.put("status", 200);
                res.put("addr", zk.getWriteServer(tName));
            }
            return res;
        }

        // Meta 信息（tableName + writable）
        @RequestMapping("/meta")
        public JSONObject getMeta() {
            JSONObject res = new JSONObject();
            res.put("data", zk.getMeta());
            return res;
        }
    }

    @RequestMapping("/")
    public String hello() {
        return "hello world";
    }

}
